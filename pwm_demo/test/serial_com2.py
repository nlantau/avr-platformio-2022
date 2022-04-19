#! /usr/bin/env python
# nlantau, 2022-04-19
# updated, 2022-04-20

import serial
import time
import tkinter as tk
from tkinter import ttk
import threading


class App(tk.Tk):

    def __init__(self):
        super().__init__()
        self.title('Serial UART - LED control')
        self.geometry('400x200+50+50')
        self.minsize(200,100)
        self.maxsize(450, 250)
        self.resizable(1,1)


class MainFrame(ttk.Frame):

    def __init__(self, container):
        super().__init__(container)

        self.parent = container
        self._is_started = False
        self.ser_th = None
        
        options = {'padx': 5, 'pady':5}

        # Label
        self.label = ttk.Label(self, text="     ", width=5)
        self.label.grid(column=2, row=0,**options)

        # Button
        self.button_start = ttk.Button(self, text="Start")
        self.button_start['command'] = self.button_clicked_start
        self.button_start.grid(column=4, row=0, **options)

        self.button_stop = ttk.Button(self, text="Stop")
        self.button_stop['command'] = self.button_clicked_stop
        self.button_stop.grid(column=4, row=1, **options)

        self.button_exit = ttk.Button(self, text="Exit", command=self.kill_app)
        self.button_exit.grid(column=4, row=2, **options)

        # User input - Sleep
        self.var_inp = tk.StringVar()
        self.var_inp.trace_add("write", self._entry_tracer)
        self.var_label = ttk.Label(self, text="Sleep (in seconds): ")
        self.var_label.grid(column=0, row=0, **options)

        self.ent_inp = ttk.Entry(self, textvariable=self.var_inp)
        self.ent_inp.grid(column=1, row=0, **options)

        # User input - Step
        self.var_inp_st = tk.StringVar()
        self.var_inp_st.trace_add("write", self._entry_tracer_st)
        self.var_label_st = ttk.Label(self, text="Step size: ")
        self.var_label_st.grid(column=0, row=1, **options)

        self.ent_inp_st = ttk.Entry(self, textvariable=self.var_inp_st)
        self.ent_inp_st.grid(column=1, row=1, **options)

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
        self.c1.grid(column=2, row=1, **options)

        self._ent_valid = ""
        self._ent_valid_st = ""

        # Show the frame on the container
        self.grid(**options)

    def _entry_tracer(self, *args):
        import re
        com1 = re.compile(r'([0-9]{1,7})(\.[0-9]+)?')
        var_len = len(self.var_inp.get())
        if 7 > var_len > 0 and com1.fullmatch(str(self.var_inp.get())):
            print(f"in - check, {self.var_inp.get()=}")
            self._ent_valid = self.var_inp.get()
        else:
            print(f"in - not valid, {self.var_inp.get()=}")
            self._ent_valid = ""

    def _entry_tracer_st(self, *args):
        import re
        com = re.compile(r'[0-9]{1,2}')
        var_len = len(self.var_inp_st.get())
        if 3 > var_len > 0 and com.fullmatch(self.var_inp_st.get()):
            print("in")
            self._ent_valid_st = self.var_inp_st.get()
        else:
            self._ent_valid_st = ""

    def kill_app(self):
        if self._is_started:
            # TODO: Fix me
            self.button_clicked_stop()
            time.sleep(1)
            self.parent.destroy()
        self.parent.destroy()


    def button_clicked_start(self):
        if not self._is_started:
            if (len(self._ent_valid) > 0 
                and len(self._ent_valid_st) > 0
                and not self.iv_c1.get()):
                print(f'{self._ent_valid=}')
                self.ser_th = SerialReader(self, sleep=self._ent_valid, step=int(self._ent_valid_st), top=255)
                self.ser_th.start()
                self._is_started = True
            elif len(self.selected_size.get()) > 0 and len(self.selected_step.get()) > 0 and self.iv_c1.get():
                print(f'{self._ent_valid=}')
                self.ser_th = SerialReader(self, sleep=self.selected_size.get(), step=int(self.selected_step.get()), top=255)
                self.ser_th.start()
                self._is_started = True

    def button_clicked_stop(self):
        if self._is_started:
            self.ser_th.stop()
            self._is_started = False


class SerialReader(threading.Thread):

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

    def run(self):
        self._transceive_th.start()

    def stop(self):
        self._do_run = False
        self.ser.cancel_read()
        self.ser.cancel_write()
        self.join()
        self.ser.close()

    def transceive(self):
        while self._do_run:
            try:
                self._runner(0, self._top, self._step, self._sleep_dur)
                self._runner(self._top - 1, -1, -self._step, self._sleep_dur)

            except serial.SerialException as e:
                if self._do_run:
                    print(e)
            except Exception:
                pass
        print("Canceled")

    def _runner(self, start, stop, step, sleep):
        for i in range(start, stop, step):
            self.ser.write(bytes(f'{i}\n', "utf-8"))

            # The AVR program is transmitting back the string is receives,
            # which is why we must readline, which we might use to set a label
            self.rx = self.ser.readline()

            self.parent.label['text'] = self.rx

            time.sleep(sleep)

if __name__ == "__main__":
    app = App()
    frame = MainFrame(app)
    app.mainloop()
