import matplotlib.pyplot as plt
import numpy as np
import csv  # importamos csv para poder leer el fichero de entrada


raw_data = 'Tracker3.csv'
with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    distancia_TX = [float(fila['Distancia_to_TX']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    azimut_TX = [float(fila['Azimuth_to_TX']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    snr_RX = [float(fila['SNR_RX']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    rssi_RX = [float(fila['RSSI_RX']) for fila in lector]

muestreo=np.arange(0,len(azimut_TX))
Fig2, [ax1, ax2,ax3,ax4] = plt.subplots(4, 1)
ax1.plot(muestreo,distancia_TX,linewidth=3)
ax1.grid(visible=True, which='major', color='#300000', linestyle='-')
ax1.minorticks_on()
ax1.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax1.set_title('Distancia hacia el objetivo',fontsize=14,style='italic')
ax1.set_xlabel('Item',fontsize=14,style='italic')
ax1.set_ylabel('metros(m)',fontsize=14,style='italic')

ax2.plot(muestreo,azimut_TX,linewidth=3)
ax2.grid(visible=True, which='major', color='#300000', linestyle='-')
ax2.minorticks_on()
ax2.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax2.set_title('Azimut hacia el objetivo',fontsize=14,style='italic')
ax2.set_xlabel('Item',fontsize=14,style='italic')
ax2.set_ylabel('degrees(°)',fontsize=14,style='italic')

ax3.plot(muestreo,snr_RX,linewidth=3)
ax3.grid(visible=True, which='major', color='#300000', linestyle='-')
ax3.minorticks_on()
ax3.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax3.set_title('Variación SNR',fontsize=14,style='italic')
ax3.set_xlabel('Item',fontsize=14,style='italic')
ax3.set_ylabel('dB',fontsize=14,style='italic')

ax4.plot(muestreo,rssi_RX,linewidth=3)
ax4.grid(visible=True, which='major', color='#300000', linestyle='-')
ax4.minorticks_on()
ax4.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax4.set_title('Variación RSSI',fontsize=14,style='italic')
ax4.set_xlabel('Item',fontsize=14,style='italic')
ax4.set_ylabel('dB',fontsize=14,style='italic')

Fig2.suptitle('Distancia=109m, SF12=-20dB, Bandwidth=250kHz, Coding rate=4/7, Automatic gain control=ON',fontsize=15,fontweight='bold')

plt.show()