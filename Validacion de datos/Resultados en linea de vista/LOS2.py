import matplotlib.pyplot as plt
import numpy as np
import cartopy.crs as ccrs
import cartopy.io.img_tiles as cimgt
import cartopy.geodesic as gd
from cartopy.mpl.ticker import LongitudeFormatter, LatitudeFormatter
import csv  # importamos csv para poder leer el fichero de entrada
import io
from urllib.request import urlopen, Request
from PIL import Image
from math import radians, sin, cos, atan2, sqrt,pow,pi,degrees

'''Prueba en linea de vista (LOS) de 2.7km, posición receptor: -0.233404 -78.475462
                                            posición transmisor: -0.231578, -78.451506

'''
#RX_pos=[-0.217129, -78.471211]
RX_pos=[-0.217360, -78.471235]
TX_pos=[-0.231578, -78.451506]

def image_spoof(self, tile):  # this function pretends not to be a Python script
    url = self._image_url(tile)  # get the url of the street map API
    req = Request(url)  # start request
    req.add_header('User-agent', 'Anaconda 3')  # add user agent to request
    fh = urlopen(req)
    im_data = io.BytesIO(fh.read())  # get image
    fh.close()  # close url
    img = Image.open(im_data)  # open image with PIL
    img = img.convert(self.desired_tile_form)  # set image format
    return img, self.tileextent(tile), 'lower'  # reformat for cartopy

def distance_between(pos_init,pos_end):
    Long1=pos_init[0]
    Long2=pos_end[0]
    Lat1=pos_init[1]
    Lat2=pos_end[1]
    Long1, Lat1, Long2, Lat2 = map(radians, [Long1, Lat1, Long2, Lat2])
    delta=Long1-Long2
    sdlong=sin(delta)
    cdlong=cos(delta)
    slat1=sin(Lat1)
    clat1=cos(Lat1)
    slat2 = sin(Lat2)
    clat2 = cos(Lat2)
    delta=(clat1*slat2)-(slat1*clat2*cdlong)
    delta=pow(delta,2)
    delta+=pow(clat2*sdlong,2)
    delta=sqrt(delta)
    denom=(slat1*slat2)+(clat1*clat2*cdlong)
    delta=atan2(delta,denom)
    return delta*6372795

def course_to(pos_init,pos_end):
    Long1 = pos_init[0]
    Long2 = pos_end[0]
    Lat1 = pos_init[1]
    Lat2 = pos_end[1]
    Long1, Lat1, Long2, Lat2 = map(radians, [Long1, Lat1, Long2, Lat2])
    dlon=Long2-Long1
    a1=sin(dlon)*cos(Lat2)
    a2=sin(Lat1)*cos(Lat2)*cos(dlon)
    a2=cos(Lat1)*sin(Lat2)-a2
    a2=atan2(a1,a2)
    if a2<0.0:
        a2+=2.0*pi
    return degrees(a2)

def Noise_floor(list_RSSI,list_SNR,N):
    Ruido=[]
    for x in N:
        Ruido.append(list_RSSI[x]-list_SNR[x])
    return Ruido

raw_data = 'LOS2,7km_POS2.csv'
with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    Conta_TX = [int(fila['Contador_TX'])-100000 for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    Item_num = [int(fila['Numero_dato'])-100000 for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    RSSI_item = [int(fila['RSSI']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    SNR_item = [float(fila['SNR']) for fila in lector]

'''CARTOGRAMA DEL LUGAR DE LA PRUEBA DE RECEPCION DE DATOS'''
cimgt.QuadtreeTiles.get_image = image_spoof # reformat web request for street map spoofing
osm_img = cimgt.QuadtreeTiles() # spoofed, downloaded street map

fig = plt.figure(figsize=(35,50)) # open matplotlib figure
rotated_crs = ccrs.RotatedPole(pole_longitude=120.0, pole_latitude=70.0)
#ax1 = plt.axes(projection=rotated_crs)
ax1 = plt.axes(projection=osm_img.crs) # project using coordinate reference system (CRS) of street map
ax1.set_title('Prueba en línea de vista', fontsize=16)

center_pt = [-0.220721, -78.462921] #

zoom = 0.0155 # for zooming out of center point
extent = [center_pt[1]-(zoom*1.1),center_pt[1]+(zoom*1.1),center_pt[0]-zoom,center_pt[0]+zoom] # adjust to zoom
print(extent)
ax1.set_extent(extent) # set extents
ax1.set_xticks(np.linspace(extent[0], extent[1], 7), crs=ccrs.PlateCarree())  # set longitude indicators
ax1.set_yticks(np.linspace(extent[2], extent[3], 7)[1:], crs=ccrs.PlateCarree())  # set latitude indicators
lon_formatter = LongitudeFormatter(number_format='0.1f', degree_symbol='', dateline_direction_label=True)  # format lons
lat_formatter = LatitudeFormatter(number_format='0.1f', degree_symbol='')  # format lats
ax1.xaxis.set_major_formatter(lon_formatter)  # set lons
ax1.yaxis.set_major_formatter(lat_formatter)  # set lats
ax1.xaxis.set_tick_params(labelsize=14)
ax1.yaxis.set_tick_params(labelsize=14)

scale = np.ceil(-np.sqrt(2)*np.log(np.divide(zoom,350.0))) # empirical solve for scale based on zoom
scale = (scale<20) and scale or 19 # scale cannot be larger than 19
ax1.add_image(osm_img, int(scale)) # add OSM with zoom specification

GPS_pos1_Long=[RX_pos[1],TX_pos[1]]
GPS_pos1_Lati=[RX_pos[0],TX_pos[0]]
geod=gd.Geodesic()
init_point=[TX_pos[1],TX_pos[0]]#[Longitud,Latitud]
end_point=[RX_pos[1],RX_pos[0]]#[Longitud,Latitud]
Dist_cartopy,azimuth1,azimuth2=np.array(geod.inverse(init_point,end_point).T)#Distancia con Cartopy
str_dist='Distancia='+str(int(Dist_cartopy))+' m'


ax1.plot(GPS_pos1_Long, GPS_pos1_Lati,'--',label=str_dist,markeredgewidth=7.0,color='magenta',lw=2.5, transform=ccrs.PlateCarree())
ax1.plot(TX_pos[1],TX_pos[0],label='Transmisor', markersize=20,markeredgewidth=5.0, marker='x', linestyle='', color='orange', transform=ccrs.PlateCarree())#Origen
ax1.plot(RX_pos[1],RX_pos[0],label='Receptor',markersize=20,markeredgewidth=5.0, marker='x', linestyle='', color='yellow', transform=ccrs.PlateCarree())#Destino
ax1.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False)
ax1.legend(loc="upper right", bbox_to_anchor=[1, 1],ncol=4, shadow=True, title=" ", fancybox=True)# El bbox_to_anchor es un punto de referencia alrededor del                                                                                           # cual se ubica la caja de leyendas segun lo diga el loc

print('Distancia con cartopy:'+str(float(Dist_cartopy))+'m')
#plt.text(TX_pos[1],TX_pos[0], 'Hello World!', fontsize=20, color='green',transform=ccrs.PlateCarree())

'''GRÁFICAS PARA ANÁLISIS DE LA CALIDAD DEL RADIOENLACE'''
Fig2, [ax1, ax2, ax3, ax4, ax5] = plt.subplots(5, 1)
n = min(Item_num)
m = max(Item_num)
Noise = []
muestreo = np.arange(0, len(Conta_TX))
muestreo = muestreo + 2
Faltan = []
F2 = []
iterar = np.arange(n, m + 1)
for i in iterar:
    F1 = i in Item_num
    if F1 == False:
        Faltan.append(i)
        F2.append(6)# 0
    else:
        F2.append(7)# 1
print(Faltan)
# Reeemplazar con un zero donde hay valores negativos
Conta_TX = np.array(Conta_TX)
Conta_TX[Conta_TX < 0] = 0

ax1.step(muestreo, Conta_TX)
ax1.grid(visible=True, which='major', color='#300000', linestyle='-')
ax1.minorticks_on()
ax1.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax1.set_title('Datos receptados')
ax1.set_xlabel('Nùmero de dato')
ax1.set_ylabel('Contador')

# ax2.scatter(muestreo,SNR_item)
# ax2.plot(muestreo,RSSI_item, drawstyle="steps")
ax2.step(muestreo, Item_num)
ax2.grid(visible=True, which='major', color='#300000', linestyle='-')
ax2.minorticks_on()
ax2.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax2.set_title('Items recibidos')
ax2.set_xlabel('N')
ax2.set_ylabel('Item')

ax3.plot(muestreo, SNR_item, linewidth=2)
ax3.grid(visible=True, which='major', color='#300000', linestyle='-')
ax3.minorticks_on()
ax3.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax3.set_title('SNR')
ax3.set_xlabel('Item')
ax3.set_ylabel('dB')
ax3.plot([-0, 310], [-20.0, -20.0], color="green", linewidth=3)
#ax3.plot([-0, 60], [-20.0, -20.0], color="green", linewidth=3)

ax4.plot(muestreo, RSSI_item, linewidth=2)
ax4.grid(visible=True, which='major', color='#300000', linestyle='-')
ax4.minorticks_on()
ax4.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax4.set_title('RSSI')
ax4.set_xlabel('Item')
ax4.set_ylabel('dB')
ax4.plot([-0, 310], [-135, -135], color="green", linewidth=3)
#ax4.plot([-0, 60], [-135, -135], color="green", linewidth=3)

muestro_RSSI = np.arange(0, len(RSSI_item))
Noise = Noise_floor(RSSI_item, SNR_item, muestro_RSSI)
ax5.plot(muestro_RSSI, Noise, linewidth=2)
ax5.grid(visible=True, which='major', color='#300000', linestyle='-')
ax5.minorticks_on()
ax5.grid(visible=True, which='minor', color='#900000', linestyle=':')
ax5.set_title('Noise floor')
ax5.set_xlabel('Item')
ax5.set_ylabel('dB')

Fig2.suptitle(str_dist+', SF12=-20dB, Bandwidth=250kHz, Coding rate=4/7, Automatic gain control=ON')

plt.show() # show the plot
