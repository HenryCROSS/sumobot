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

from enum import Flag

class Stage(Flag):
    STOP = 1
    MONITOR = 2


class GLOBAL:
    port = "COM3"
    new_port = "COM3"
    baudrate = 9600
    new_baudrate = 9600
    data = []
    is_running = True
    clear = False
    interval = 1/60
    monitoring_rate = 0.0001 # sec
    connection_success = True
    stage = Stage.STOP
    

def monitor_port():
    while GLOBAL.is_running:
        match GLOBAL.stage:
            case Stage.MONITOR:
                try:
                    with serial.Serial(GLOBAL.port, GLOBAL.baudrate, timeout= 1) as ser:
                        GLOBAL.connection_success = True
                        while GLOBAL.stage is Stage.MONITOR and GLOBAL.is_running:
                            line = ser.readline()

                            GLOBAL.data.append(line.decode().rstrip())
                            time.sleep(GLOBAL.monitoring_rate)
                except:
                    GLOBAL.connection_success = False
                    time.sleep(1)
            case Stage.STOP:
                time.sleep(1)

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
    def compose(self) -> ComposeResult:
        with Horizontal():
            with Horizontal(id="filterBar"):
                yield Input(placeholder="Filter", id="filter")
            with Horizontal():
                yield Input(placeholder="File: data.log", id="file")
                yield Button("Save", id="save")
                yield Button("Start", id="start", variant="success")
                yield Button("Stop", id="stop", variant="warning")
                yield Button("Clear", id="clear", variant="error")
        
    def on_mount(self):
        self.styles.dock = "top"
        self.styles.height = 4
        self.query_one("#file").styles.width = "50%"
        self.query_one("#filterBar").styles.width = "30%"
        
        self.update_styles = self.set_interval(GLOBAL.interval, self.update_style)
            
        
        self.filter_content = ""
        self.file_name = ""
    
    @on(Input.Changed)
    def update_input_content(self, event: Input.Changed):
        if event.input.id == "filter":
            self.filter_content = event.value
        elif event.input.id == "file":
            self.file_name = event.value
    
    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "save":
            self.save_file()
        elif event.button.id == "start":
            self.add_class("started")
            GLOBAL.stage = Stage.MONITOR
        elif event.button.id == "stop":
            self.remove_class("started")
            GLOBAL.stage = Stage.STOP
        elif event.button.id == "clear":
            GLOBAL.clear = True
        
    def get_filter_content(self) -> str:
        return self.filter_content
    
    def get_file_name(self) -> str:
        return self.file_name
    
    def save_file(self):
        if self.file_name == "":
            self.file_name = "data.log"
        with open(self.file_name, 'w+') as file:
            pattern = self.filter_content
            output_lines = filter(lambda line: pattern == "" or re.search(pattern, line) is not None, GLOBAL.data)
            output_lines = map(lambda line: line + "\n", output_lines)
            file.writelines(output_lines)
    
    def update_style(self):
        start = self.query_one("#start")
        stop = self.query_one("#stop")
        
        if(self.has_class("started")):
            start.styles.display = "none"
            stop.styles.display = "block"
        else:
            stop.styles.display = "none"
            start.styles.display = "block"
        

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

    def compose(self):
        with Container():
            with Vertical():
                yield TopPart()
                yield BottomPart()

    def on_mount(self):
        self.update_filter_content = self.set_interval(GLOBAL.interval, self.get_filter_content)
        self.update_log = self.set_interval(GLOBAL.interval, self.update_log)
        self.update_setting = self.set_interval(GLOBAL.interval, self.update_settings)
        
        self.state = {
            "filter_content": "",
            "file_name": "",
            "log_line": 0,
        }
        
    def update_log(self):
        if GLOBAL.clear:
            GLOBAL.clear = False
            self.state["log_line"] = 0
            self.query_one(BottomPart).clear_nonfiltered_log()
            self.query_one(BottomPart).clear_filtered_log()
            
            GLOBAL.data.clear()
            
        
        length = len(GLOBAL.data)
        while self.state["log_line"] < length:
            line = GLOBAL.data[self.state["log_line"]]
            self.state["log_line"] += 1
            
            self.query_one(BottomPart).write_nonfiltered_log(line)
            
            pattern = self.state['filter_content']
            if (pattern == "" or re.search(pattern, line) is not None):
                self.query_one(BottomPart).write_filtered_log(line)
                
    def update_settings(self):
        if GLOBAL.stage is Stage.STOP:
            GLOBAL.baudrate = GLOBAL.new_baudrate
            GLOBAL.port = GLOBAL.new_port
        
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
            if event.value == "":
                GLOBAL.new_port = "COM3"
            else:
                GLOBAL.new_port = event.value
        elif event.input.id == "baudrate":
            if event.value == "":
                GLOBAL.new_baudrate = 9600
            else:
                GLOBAL.new_baudrate = event.value
    
    def on_mount(self):
        self.update_port_list = self.set_interval(GLOBAL.interval, self.update_port_list)
    
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
        self.update_title = self.set_interval(GLOBAL.interval, self.update_header)
    
    def update_header(self):
        title = f"port: {GLOBAL.port} | baudrate: {GLOBAL.baudrate}"
        if GLOBAL.stage is Stage.MONITOR and (GLOBAL.new_port != GLOBAL.port or GLOBAL.new_baudrate != GLOBAL.baudrate):
            title = f"port: {GLOBAL.port} -> {GLOBAL.new_port} | baudrate: {GLOBAL.baudrate} -> {GLOBAL.new_baudrate} | need to stop first"
        self.app.screen.title = title
        
        if not GLOBAL.connection_success:
            self.app.screen.sub_title = "[Connection Failed]"
        elif GLOBAL.stage is Stage.MONITOR:
            self.app.screen.sub_title = "[Monitoring]"
        elif GLOBAL.stage is Stage.STOP:
            self.app.screen.sub_title = "[Stop]"
    

if __name__ == "__main__":
    # start a thread
    handle = threading.Thread(group=None, target=monitor_port)
    handle.start()
    
    ContentSwitcherApp().run()
    GLOBAL.is_running = ContentSwitcherApp().is_running
    
    handle.join()