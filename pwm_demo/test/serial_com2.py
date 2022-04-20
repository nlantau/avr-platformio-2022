#! /usr/bin/env python
# nlantau, 2022-04-19
# updated, 2022-04-20

import logging
import re
from turtle import bgcolor
import serial
import serial.tools.list_ports
import time
import threading
import tkinter as tk
from tkinter import ttk
import time


class App(tk.Tk):

    def __init__(self):
        log_name = f'{time.strftime("%Y-%m-%d", time.localtime())}'
        logging.basicConfig(filename=f'{log_name}.log', level=logging.DEBUG)
        logging.info(f"Application started {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}")
        super().__init__()
        self.title('Serial UART - LED control')
        self.geometry('500x200+50+50')
        self.minsize(200,100)
        self.maxsize(550, 250)
        self.resizable(1,1)


class MainFrame(ttk.Frame):

    def __init__(self, container):
        super().__init__(container)

        self.parent = container
        self._is_started = False
        self.ser_th = None
        self._float_regex = re.compile(r'([0-9]{1,7})(\.[0-9]+)?')
        self._int_regex = re.compile(r'[0-9]{1,2}')
        
        options = {'padx': 5, 'pady':5}

        # Label
        self.label = ttk.Label(self, text="     ", width=20, borderwidth=1, relief='ridge')
        self.label.grid(column=2, row=0,**options, sticky='nw')

        # Button
        self.button_start = ttk.Button(self, text="Start")
        self.button_start['command'] = self.button_clicked_start
        self.button_start.grid(column=4, row=0, **options, sticky='ne')

        self.button_stop = ttk.Button(self, text="Stop")
        self.button_stop['command'] = self.button_clicked_stop
        self.button_stop.grid(column=4, row=1, **options, sticky='ne')

        self.button_exit = ttk.Button(self, text="Exit", command=self.kill_app)
        self.button_exit.grid(column=4, row=2, **options, sticky='ne')

        # User input - Sleep
        self.var_inp = tk.StringVar()
        self.var_inp.trace_add("write", self._entry_tracer)
        self.var_label = ttk.Label(self, text="Sleep (in seconds): ")
        self.var_label.grid(column=0, row=0, **options, sticky='nw')

        self.ent_inp = ttk.Entry(self, textvariable=self.var_inp)
        self.ent_inp.grid(column=1, row=0, **options, sticky='nw')

        # User input - Step
        self.var_inp_st = tk.StringVar()
        self.var_inp_st.trace_add("write", self._entry_tracer_st)
        self.var_label_st = ttk.Label(self, text="Step size: ")
        self.var_label_st.grid(column=0, row=1, **options, sticky='nw')

        self.ent_inp_st = ttk.Entry(self, textvariable=self.var_inp_st)
        self.ent_inp_st.grid(column=1, row=1, **options, sticky='nw')

        # Radio buttons - Sleep
        self.selected_size = tk.StringVar()
        self.fast = ttk.Radiobutton(self, text="Fast", value="0.01", variable=self.selected_size)
        self.fast.grid(column=0, row=2, sticky="nw")
        self.med = ttk.Radiobutton(self, text="Medium", value="0.1", variable=self.selected_size)
        self.med.grid(column=0, row=3, sticky="nw")
        self.slow = ttk.Radiobutton(self, text="Slow", value="1", variable=self.selected_size)
        self.slow.grid(column=0, row=4, sticky="nw")

        # Radio buttons - Step
        self.selected_step = tk.StringVar()
        self.s1 = ttk.Radiobutton(self, text="Step 1", value="1", variable=self.selected_step)
        self.s1.grid(column=1, row=2, sticky="nw")
        self.s2 = ttk.Radiobutton(self, text="Step 2", value="2", variable=self.selected_step)
        self.s2.grid(column=1, row=3, sticky="nw")
        self.s3 = ttk.Radiobutton(self, text="Step 5", value="5", variable=self.selected_step)
        self.s3.grid(column=1, row=4, sticky="nw")

        # Checkbox
        self.iv_c1 = tk.IntVar()
        self.c1 = ttk.Checkbutton(self, text="Use preset", variable=self.iv_c1, onvalue=1, offvalue=0, width=10)
        self.c1.grid(column=2, row=1, **options, sticky='nw')

        # Circle
        self.canvas = tk.Canvas(self, width=15, height=15)
        self.canvas.grid(column=2, row=2, **options, sticky='nw')
        self.circle = self.canvas.create_oval(1,1,10,10, fill='blue', tags='circ')

        # List ports
        self.cbb_portscombo = ttk.Combobox(self, postcommand=self.get_ports)
        self.cbb_portscombo.grid(column=0, row=5, **options, sticky='nw')
        self.lbl_cbb_choice = ttk.Label(self, text="", width=20)
        self.lbl_cbb_choice.grid(column=1, row=5, **options, sticky='nw')

        self.cbb_portscombo.bind('<<ComboboxSelected>>',
            lambda c: self.lbl_cbb_choice.config(text=self._get_port_desc(c.widget.get())))

        # Display Errors
        self.lbl_error = ttk.Label(self, text="", width=20)
        self.lbl_error.grid(column=0, row=6, **options, sticky='nw')

        # User input placeholders
        self._ent_valid = ""
        self._ent_valid_st = ""

        # Show the frame on the container
        self.grid(**options)


    def get_ports(self) -> None:
        self.ports = serial.tools.list_ports.comports()
        self.cb_dict = dict()
        for port, desc, hwid in sorted(self.ports):
            self.cb_dict[port] = desc
        self.cbb_portscombo['values'] = list(self.cb_dict.keys())
    
    def _get_port_desc(self, port):
        return self.cb_dict[port]

    def set_circle_color(self, color):
        self.canvas.itemconfig(self.circle, fill=color)

    def _entry_tracer(self, *args):
        var_len = len(self.var_inp.get())
        if 7 > var_len > 0 and self._float_regex.fullmatch(str(self.var_inp.get())):
            logging.debug(f"in - check, {self.var_inp.get()=}")
            self._ent_valid = self.var_inp.get()
        else:
            logging.debug(f"in - not valid, {self.var_inp.get()=}")
            self._ent_valid = ""

    def _entry_tracer_st(self, *args):
        var_len = len(self.var_inp_st.get())
        if 3 > var_len > 0 and self._int_regex.fullmatch(self.var_inp_st.get()):
            logging.debug("in")
            self._ent_valid_st = self.var_inp_st.get()
        else:
            self._ent_valid_st = ""

    def kill_app(self):
        if self._is_started:
            # TODO: Fix me
            self.button_clicked_stop()
            time.sleep(1)
        self.parent.destroy()
        logging.info(f"Application stopped {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())}")

    def button_clicked_start(self):
        if not self._is_started and len(self.lbl_cbb_choice['text']) > 0:
            if (len(self._ent_valid) > 0 and len(self._ent_valid_st) > 0 and not self.iv_c1.get()):
                sleep = self._ent_valid
                step = int(self._ent_valid_st)
                top = 255
                logging.debug(f"{sleep=},{step=},{top=}")
            elif len(self.selected_size.get()) > 0 and len(self.selected_step.get()) > 0 and self.iv_c1.get():
                sleep = self.selected_size.get()
                step = int(self.selected_step.get())
                top = 255
                logging.debug(f"{sleep=},{step=},{top=}")
            else:
                sleep, step, top, = 1, 1, 1
                logging.debug(f"{sleep=},{step=},{top=}")

            port = self.cbb_portscombo.get()
            if self._get_port_desc(port) == 'n/a':
                self.lbl_error['background'] = 'red'
                self.lbl_error['text'] = "Can't use port"
            else:
                self.ser_th = SerialReader(self, PORT=port, sleep=sleep, step=step, top=top)
                self.ser_th.start()
                self._is_started = True
                self.lbl_error['text'] = "Port OK"
                self.lbl_error['background'] = 'green'

    def button_clicked_stop(self):
        if self._is_started:
            self.ser_th.stop()
            self._is_started = False


class SerialReader(threading.Thread):
    """Handler for serial communication. Runs on separate thread"""

    def __init__(self, parent, PORT='/dev/ttyUSB0', BAUD=9600, sleep=0.1, step=1, top=256):
        super().__init__()
        self.parent = parent
        self.PORT = PORT
        self.BAUD = BAUD
        self._do_run = True
        self._transceive_th = threading.Thread(target=self.transceive)

        self._sleep_dur = float(sleep)
        self._step = step
        self._top = top

        self.ser = serial.Serial(self.PORT, self.BAUD, parity=serial.PARITY_EVEN, stopbits=2, timeout=0.5)
        logging.debug(f'{self.PORT=},{self.BAUD=},{self._sleep_dur=},{self._step=},{self._top=}')

    def run(self):
        logging.info(f"Starting {self._transceive_th.name}")
        self._transceive_th.start()

    def stop(self):
        self._do_run = False
        self.ser.cancel_read()
        self.ser.cancel_write()
        logging.info(f"Joining {self._transceive_th.name}")
        self.join()
        logging.info(f"Closing {self.ser.name}")
        try:
            self.ser.close()
        except Exception as e:
            logging.error(f'stop(): {e}')

    def transceive(self):
        while self._do_run:
            try:
                self._runner(0, self._top, self._step, self._sleep_dur)
                self._runner(self._top - 1, -1, -self._step, self._sleep_dur)

            except serial.SerialException as e:
                if self._do_run:
                    self.parent.lbl_error['text'] = e
                    logging.error(f'self._do_run: {e}')
            except Exception as e:
                logging.error(f'Exception: {e}')
        logging.info("Stop running")

    def _runner(self, start, stop, step, sleep):
        logging.debug(f'{start=},{stop=},{step=},{sleep=}')
        for i in range(start, stop, step):

            # Transmit i + \n (PWM value, 8-bit: 0 <= i < 256)
            self.ser.write(bytes(f'{i}\n', "utf-8"))

            # The AVR program is transmitting back the string is receives,
            # which is why we must readline, which we might use to set a label
            self.rx = self.ser.readline()

            # Update GUI
            self.parent.label['text'] = f'PWM: {self.rx.decode().strip()}'
            self.parent.set_circle_color(f'#0{str(i)[-1]}{str(i)[0]}')

            time.sleep(sleep)

if __name__ == "__main__":
    app = App()
    frame = MainFrame(app)
    app.mainloop()
