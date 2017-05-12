from Tkinter import *
import sys
import glob
import ttk
import tkFileDialog
import serial
from serial.tools import list_ports
import math
import matplotlib as mpl
mpl.use('TkAgg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import  FigureCanvasTkAgg, NavigationToolbar2TkAgg
	

class mainProgramme(Tk):
	def setGraph(self):
		self.spAmpl.cla()
		self.spAmpl.clear()
		self.spAmpl.grid(b=True, which='both', axis='both')
		self.spAmpl.set_xbound(self.freq_min-(self.freq_min+self.freq_max)/2, self.freq_max-(self.freq_min+self.freq_max)/2)
		
		self.spPhas.cla()
		self.spPhas.clear()
		self.spPhas.grid(b=True, which='both', axis='both')
		self.spPhas.set_xbound(self.freq_min-(self.freq_min+self.freq_max)/2, self.freq_max-(self.freq_min+self.freq_max)/2)
	
	def toggleScan(self):
		if(self.scan==0):
			del(self.liste_frequences[:])
			del(self.liste_amplitudes[:])
			del(self.liste_phases[:])
			del(self.results[:])
			self.spAmpl.cla()
			self.spAmpl.clear()
			self.spPhas.cla()
			self.spPhas.clear()
			
			self.myPort=serial.Serial(self.Uart.get(), 115200);
			self.freq_min=int(self.spinDo.get())
			self.freq_max=int(self.spinUp.get())
			self.freq_pas=int(self.spinPas.get())
			self.freq_pps=int(self.spinPps.get())
			self.myPort.flushInput()
			self.myPort.write(str(self.freq_min))
			self.myPort.write('/');
			self.myPort.write(str(self.freq_max))
			self.myPort.write('/');
			self.myPort.write(str(self.freq_pas))
			self.myPort.write('/');
			self.myPort.write(str(1000/self.freq_pps))
			self.myPort.write('/');
			self.myPort.write(str(self.spinMoy.get()))
			self.myPort.write('\r');
			self.scan=1
			self.labeVoy.config(text="Scanning...")
			self.boutTog.config(text="Stop")
			self.acquire()
			self.setGraph()

		else:
			self.myPort.write('\x03');
			self.myPort.close;
			self.scan=0
			self.labeVoy.config(text="Waiting...")
			self.boutTog.config(text="Start")

		
	def acquire(self):
		if(self.scan==1):
			newStr=self.myPort.readline()
			self.results.append(newStr)
			
			if newStr=="END\r\n":
				self.myPort.close()
				self.scan=0
				self.labeVoy.config(text="Finished !")
				self.boutTog.config(text="Start")
				self.spPhas.set_ylabel('Phase (deg).', fontdict={'fontsize':10})
				self.spPhas.set_xlabel('freq. offset (Hz)', fontdict={'fontsize':10})
				self.spAmpl.set_ylabel('Amplitude (a.u.)', fontdict={'fontsize':10})
				self.spAmpl.set_xlabel('freq. offset (Hz)', fontdict={'fontsize':10})
				self.cvPlot.show()
			else:
				values=newStr.split("/");
				if(len(values)==3):
					amp=float((int(values[0], 16)*11)/273.0)#(U*3300/4095)/20 
					pha=float((int(values[1], 16)*4)/91.0)#(U/4095)*180 
					self.liste_amplitudes.append(amp)
					self.liste_phases.append(pha)
					
					freq=((int(values[2],16)*546875)>>21)-(self.freq_min+self.freq_max)/2
					self.liste_frequences.append(freq)
					self.setGraph()
					self.spAmpl.plot(self.liste_frequences,self.liste_amplitudes,'b')
					self.spPhas.plot(self.liste_frequences,self.liste_phases,'b')
					self.cvPlot.show()
				self.after(1, self.acquire)		
		else:
			del(self.liste_frequences[:])
			del(self.liste_amplitudes[:])
			del(self.liste_phases[:])
			del(self.results[:])
			self.spAmpl.cla()
			self.spAmpl.clear()
			self.spPhas.cla()
			self.spPhas.clear()
			
	def save(self):
		file_path_string = tkFileDialog.asksaveasfilename(defaultextension='.txt', filetypes=[("Text files", ".txt"), ("All files", ".*") ])
		if(file_path_string!=""):
			myFile=open(file_path_string, "w")
			myFile.write("#StartFreq(Hz)/EndFreq(Hz)/FreqStep(Hz)/FreqDelay(ms)/Average(n)\n")
			myFile.write("#Mag(bits)/Pha(bits)/freq(bits)\n")
			i=0
			while i < len(self.results):
				myFile.write(self.results[i])
				i += 1
			
			
	def __init__(self):
		Tk.__init__(self)
		self.title("Interface")
		self.geometry("750x600")
		self.Uart = StringVar()
		
		self.scan=0
		
		self.liste_frequences = []
		self.liste_amplitudes = []
		self.liste_phases = []
		self.results = []
		
		self.fgBode = Figure(figsize=(5,6), dpi=100)
		self.fgBode.suptitle('Bode plot', fontsize=12)
		self.fgBode.subplots_adjust(hspace=0.28)
		
		
		self.spAmpl = self.fgBode.add_subplot(211)
		self.spAmpl.autoscale(enable=False, axis='x')
		self.spAmpl.grid(b=True, which='both', axis='both')
		
		
		self.spPhas = self.fgBode.add_subplot(212)
		self.spPhas.autoscale(enable=False, axis='x')
		self.spPhas.grid(b=True, which='both', axis='both')
		
		frPlt = Frame(self)
		frPlt.grid(row=0, column=1,rowspan=2,padx=3,pady=5)
		
		self.cvPlot = FigureCanvasTkAgg(self.fgBode, master=frPlt)
		self.cvPlot.show()
		self.cvPlot.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)
		
		frUser = Frame(self)
		frUser.grid(row=0, column=0,rowspan=2,padx=5,pady=5)
		
		labePer = Label(frUser, text="Peripheral")
		labePer.pack()
		uartLst=[port[0] for port in serial.tools.list_ports.comports()]
		self.spinPer = ttk.Combobox(frUser, textvariable = self.Uart, values = uartLst, state = 'readonly')
		self.spinPer.set(uartLst[0])
		self.spinPer.pack()		
		
		labeDo = Label(frUser, text="Start frequency (Hz)")
		labeDo.pack()
		self.spinDo = Spinbox(frUser, from_=50, to=19999999, value=3577000)
		self.spinDo.pack()
		
		labeUp = Label(frUser, text="Stop frequency (Hz)")
		labeUp.pack()
		self.spinUp = Spinbox(frUser, from_=51, to=20000000, value=3579000)
		self.spinUp.pack()
		
		labePas = Label(frUser, text="Frequency step (Hz)")
		labePas.pack()
		self.spinPas = Spinbox(frUser, from_=1, to=10000000, value=20)
		self.spinPas.pack()
		
		labeMoy = Label(frUser, text="Average")
		labeMoy.pack()
		self.spinMoy = Spinbox(frUser, from_=1, to=1000000)
		self.spinMoy.pack()
		
		labePps = Label(frUser, text="Measures/sec")
		labePps.pack()
		self.spinPps = Spinbox(frUser, from_=1, to=1000, value=10)
		self.spinPps.pack()
		
		frVoy = Frame(frUser)
		frVoy.pack()
		self.labeVoy = Label(frVoy, text="Waiting...")
		self.labeVoy.pack(side=LEFT)
		
		self.boutTog = Button(frUser, text='Start', command=self.toggleScan)
		self.boutTog.pack()
		
		bout1 = Button(frUser, text='Export datas', command=self.save)
		bout1.pack()
		
		bout2 = Button(frUser, text='Exit', command=self.quit)
		bout2.pack()

if(__name__ == '__main__'):
	application = mainProgramme()
	application.after(750, application.acquire)
	application.mainloop()
