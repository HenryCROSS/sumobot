# README:
# To run this script, you need to install 2 libs
# pip install pyserial
# pip install textual

from textual import on
from textual.app import App, ComposeResult
from textual.containers import Container, ScrollableContainer, Horizontal, Vertical
from textual.reactive import reactive
from textual.widgets import Button, Footer, Header, Static, Input, Log, ContentSwitcher, Label, ListItem, ListView

import threading
import serial
import serial.tools.list_ports
import re
import time

port = "COM3"
new_port = "COM3"
baudrate = 9600
new_baudrate = 9600
data = []
is_running = True
restart = False
interval = 1/60
monitoring_rate = 0.0001 # sec

def cmd_monitor_port():
    while is_running:
        global restart
        restart = False
        try:
            with serial.Serial(port, baudrate, timeout= 1) as ser:
                while not restart and is_running:
                    line = ser.readline()

                    data.append(line.decode().rstrip())
                    time.sleep(monitoring_rate)
        except:
            time.sleep(monitoring_rate)

class OutputConsole(Static):    
    def compose(self) -> ComposeResult:
        yield Log()
    
    def write_log(self, line):
        log = self.query_one(Log)
        log.write_line(line)
    
    def clear(self):
        log = self.query_one(Log)
        log.clear()

class TopPart(Static):
    filter_content = ""
    file_name = ""
    
    def compose(self) -> ComposeResult:
        with Horizontal():
            with Horizontal(id="filterBar"):
                yield Input(placeholder="Filter", id="filter")
            with Horizontal():
                yield Input(placeholder="File: data.log", id="file")
                yield Button("Save", id="save")
                yield Button("Restart", id="restart")
        
    def on_mount(self):
        self.styles.dock = "top"
        self.styles.height = 4
        self.query_one("#file").styles.width = "50%"
        self.query_one("#filterBar").styles.width = "30%"
        
        self.query_one("#filter")
    
    @on(Input.Changed)
    def update_input_content(self, event: Input.Changed):
        if event.input.id == "filter":
            self.filter_content = event.value
        elif event.input.id == "file":
            self.file_name = event.value
    
    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "save":
            self.save_file()
        elif event.button.id == "restart":
            global restart
            restart = True
        
    def get_filter_content(self) -> str:
        return self.filter_content
    
    def get_file_name(self) -> str:
        return self.file_name
    
    def save_file(self):
        if self.file_name == "":
            self.file_name = "data.log"
        with open(self.file_name, 'w+') as file:
            pattern = self.filter_content
            output_lines = filter(lambda line: pattern == "" or re.search(pattern, line) is not None, data)
            output_lines = map(lambda line: line + "\n", output_lines)
            file.writelines(output_lines)
        

class BottomPart(Static):
    def compose(self) -> ComposeResult:
        with Horizontal():
            with Vertical():
                yield OutputConsole(id="nonFiltered")
            with Vertical():
                yield OutputConsole(id="Filtered")
    
    def write_filtered_log(self, line):
        self.query_one("#Filtered").write_log(line)
        
    def write_nonfiltered_log(self, line):
        self.query_one("#nonFiltered").write_log(line)
        
    def clear_filtered_log(self):
        self.query_one("#Filtered").clear()
    
    def clear_nonfiltered_log(self):
        self.query_one("#nonFiltered").clear()


class MainPage(Static):
    state = {
        "filter_content": "",
        "file_name": "",
        "log_line": 0,
    }
    
    def compose(self):
        with Container():
            with Vertical():
                yield TopPart()
                yield BottomPart()

    def on_mount(self):
        self.update_filter_content = self.set_interval(interval, self.get_filter_content)
        self.update_log = self.set_interval(interval, self.update_log)
        
    def update_log(self):
        if restart:
            self.state["log_line"] = 0
            self.query_one(BottomPart).clear_nonfiltered_log()
            self.query_one(BottomPart).clear_filtered_log()
            data.clear()
            
            global baudrate, new_baudrate, port, new_port
            
            baudrate = new_baudrate
            port = new_port
        
        length = len(data)
        while self.state["log_line"] < length:
            line = data[self.state["log_line"]]
            self.state["log_line"] += 1
            
            self.query_one(BottomPart).write_nonfiltered_log(line)
            
            pattern = self.state['filter_content']
            if (pattern == "" or re.search(pattern, line) is not None):
                self.query_one(BottomPart).write_filtered_log(line)
                
        
    def get_filter_content(self):
        new_content = self.query_one(TopPart).get_filter_content()
        if(new_content is not self.state['filter_content']):
            self.state['filter_content'] = new_content

                
        
class ConfigTable(Static):
    def compose(self):
        yield Label("Port")
        yield Input(placeholder="COM3", id="port")
        yield Label("Baudrate")
        yield Input(placeholder="9600", id="baudrate")
        yield Label(" ")
        yield Label("Port list:")
        yield Log()

    
    @on(Input.Changed)
    def update_input_content(self, event: Input.Changed):
        if event.input.id == "port":
            global new_port
            if event.value == "":
                new_port = "COM3"
            else:
                new_port = event.value
        elif event.input.id == "baudrate":
            global new_baudrate
            if event.value == "":
                new_baudrate = 9600
            else:
                new_baudrate = event.value
    
    def on_mount(self):
        self.update_port_list = self.set_interval(interval, self.update_port_list)
    
    def update_port_list(self):
        log = self.query_one(Log)
        log.clear()
        for p in serial.tools.list_ports.comports():
            log.write_line(str(p))
        
        
class ContentSwitcherApp(App[None]):
    
    BINDINGS = [
        ('f1', "switch_page", "Open/Close Config")
    ]
    
    def compose(self):
        yield Header()
        yield Footer()
        with ContentSwitcher(initial="config"):
            yield ConfigTable(id="config")
            yield MainPage(id="main")
        
    def action_switch_page(self) -> None:
        if self.query_one(ContentSwitcher).current == "main":
            self.query_one(ContentSwitcher).current = "config"
        else:
            self.query_one(ContentSwitcher).current = "main"
    
    def on_mount(self):
        update_title = self.set_interval(interval, self.update_header)
    
    def update_header(self):
        title = f"port: {port} | baudrate: {baudrate}"
        if new_port != port or new_baudrate != baudrate:
            title = f"port: {port} -> {new_port} | baudrate: {baudrate} -> {new_baudrate} | need to restart"
        self.app.screen.title = title
    

if __name__ == "__main__":
    # start a thread
    handle = threading.Thread(group=None, target=cmd_monitor_port)
    handle.start()
    
    ContentSwitcherApp().run()
    is_running = ContentSwitcherApp().is_running
    
    handle.join()