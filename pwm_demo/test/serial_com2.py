#! /usr/bin/env python
# nlantau, 2022-04-19

from concurrent.futures import thread
import serial
import time
import tkinter as tk
from tkinter import ttk
import threading



class App(tk.Tk):

    def __init__(self):
        super().__init__()
        self.title('Serial UART')
        self.geometry('800x600')



class MainFrame(ttk.Frame):

    def __init__(self, container):
        super().__init__(container)

        self.parent = container
        self._is_started = False
        self.ser_th = None
        
        options = {'padx': 5, 'pady':5}

        # Label
        self.label = ttk.Label(self, text="")
        self.label.pack(**options)

        # Button
        self.button_start = ttk.Button(self, text="Start")
        self.button_start['command'] = self.button_clicked_start
        self.button_start.pack(**options, side=tk.LEFT)

        self.button_stop = ttk.Button(self, text="Stop")
        self.button_stop['command'] = self.button_clicked_stop
        self.button_stop.pack(**options, side=tk.LEFT)

        self.button_exit = ttk.Button(self, text="Exit", command=self.parent.destroy)
        self.button_exit.pack(**options, side=tk.LEFT)


        # Radio buttons
        self.selected_size = tk.StringVar()
        self.fast = ttk.Radiobutton(self, text="Fast", value="0.001", variable=self.selected_size).pack(side=tk.LEFT)
        self.med = ttk.Radiobutton(self, text="Medium", value="0.01", variable=self.selected_size).pack(side=tk.LEFT)
        self.slow = ttk.Radiobutton(self, text="Slow", value="0.1", variable=self.selected_size).pack(side=tk.LEFT)


        # Show the frame on the container
        self.pack(**options)




    def button_clicked_start(self):
        if not self._is_started and len(self.selected_size.get()) > 0:
            self.ser_th = SerialReader(self, sleep=self.selected_size.get())
            self.ser_th.start()
            self._is_started = True

    def button_clicked_stop(self):
        if self._is_started:
            self.ser_th.stop()
            self._is_started = False


class SerialReader(threading.Thread):

    def __init__(self, parent, PORT='/dev/ttyUSB0', BAUD=9600, sleep=0.1):
        #threading.Thread.__init__(self)
        super().__init__()
        self.parent = parent
        self.PORT = PORT
        self.BAUD = BAUD
        self._do_run = True
        self._transceive_th = threading.Thread(target=self.transceive)

        self._sleep_dur = float(sleep)


        self.ser = serial.Serial(self.PORT, self.BAUD, parity=serial.PARITY_EVEN, stopbits=2, timeout=0.5)

    def run(self):
        self._transceive_th.start()

    def stop(self):
        self._do_run = False
        print(f'{self.rx}, {self._do_run} {self._transceive_th.name}')
        self.ser.cancel_read()
        self.ser.cancel_write()
        self.join()
        self.ser.close()

    def transceive(self):
        while self._do_run:
            try:
                for i in range(256):
                    inp = f'{i}\n'
                    inp = bytes(inp, "utf-8")

                    self.ser.write(inp)
                    self.rx = self.ser.readline()
                    print(f'{self.rx}, {self._do_run} {self._transceive_th.getName()}')

                    self.parent.label['text'] = self.rx

                    time.sleep(self._sleep_dur)
                for i in range(255, -1, -1):
                    inp = f'{i}\n'
                    inp = bytes(inp, "utf-8")

                    self.ser.write(inp)
                    self.rx = self.ser.readline()
                    print(f'{self.rx}, {self._do_run}')
                    self.parent.label['text'] = self.rx

                    time.sleep(self._sleep_dur)
            except serial.SerialException as e:
                if self._do_run:
                    print(e)
            except Exception:
                pass

            print("Canceled")

if __name__ == "__main__":
    app = App()
    frame = MainFrame(app)
    app.mainloop()
