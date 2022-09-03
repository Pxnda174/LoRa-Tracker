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

RX_pos=[-0.211375,-78.488365]
TX_pos=[-0.212457, -78.489753]

raw_data = 'Tracker4.csv'
with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    LatitudRx = [float(fila['Latitud_RX']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    LongitudRx = [float(fila['Longitud_RX']) for fila in lector]

with open(raw_data) as fichero:
    lector = csv.DictReader(fichero, delimiter=',')
    Azimut_target = [float(fila['Azimuth_to_TX']) for fila in lector]

cimgt.QuadtreeTiles.get_image = image_spoof # reformat web request for street map spoofing
osm_img = cimgt.QuadtreeTiles()# spoofed, downloaded street map

fig = plt.figure(figsize=(24,18)) # open matplotlib figure
rotated_crs = ccrs.RotatedPole(pole_longitude=120.0, pole_latitude=70.0)
#ax1 = plt.axes(projection=rotated_crs)
ax1 = plt.axes(projection=osm_img.crs) # project using coordinate reference system (CRS) of street map
ax1.set_title('Tarea de búsqueda (Resultados)', fontsize=25,fontweight='bold')

#center_pt = [-0.232321, -78.455674] # lat/lon of One World Trade Center in NYC
center_pt = [-0.211895, -78.489030] # lat/lon of One World Trade Center in NYC
zoom = 0.00110 # for zooming out of center point
extent = [center_pt[1]-(zoom*1.5),center_pt[1]+(zoom*2.0),center_pt[0]-zoom,center_pt[0]+zoom] # adjust to zoom
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
ax1.plot(LongitudRx, LatitudRx,label='Ubicación del receptor', markersize=5, marker='o', linestyle='', markeredgecolor='blue', color='cyan', transform=ccrs.PlateCarree())

geod=gd.Geodesic()
init_point=[RX_pos[1],RX_pos[0]]#[Longitud,Latitud]
end_point=[TX_pos[1],TX_pos[0]]#[Longitud,Latitud]
Dist_cartopy,azimuth1,azimuth2=np.array(geod.inverse(init_point,end_point).T)#Distancia con Cartopy
Dist_vincenty=distance_between(init_point,end_point)
Azimuth_forward=course_to(init_point,end_point)
str_dist1='Distancia1='+str(int(Dist_vincenty))+'m '+'/ Azimut='+str(int(Azimuth_forward))+'°'
Dist_vincenty=distance_between([-78.489166,-0.211203],end_point)
Azimuth_forward=course_to([-78.489166,-0.211203],end_point)
str_dist2='Distancia2='+str(int(Dist_vincenty))+'m '+'/ Azimut='+str(int(Azimuth_forward))+'°'

GPS_pos1_Long=[RX_pos[1],TX_pos[1]]
GPS_pos1_Lati=[RX_pos[0],TX_pos[0]]
ax1.plot([RX_pos[1],TX_pos[1]],[RX_pos[0],TX_pos[0]],'--',label=str_dist1,markeredgewidth=7.0,color='black',lw=2.5, transform=ccrs.PlateCarree())
ax1.plot([-78.489166,TX_pos[1]],[-0.211203,TX_pos[0]],'--',label=str_dist2,markeredgewidth=7.0,color='orange',lw=2.5, transform=ccrs.PlateCarree())
ax1.plot(TX_pos[1],TX_pos[0],label='Transmisor', markersize=20,markeredgewidth=5.0, marker='x', linestyle='', color='orange', transform=ccrs.PlateCarree())#Origen
ax1.plot(RX_pos[1],RX_pos[0],label='RX/POS1',markersize=20,markeredgewidth=1.0, marker='^', linestyle='', color='purple', transform=ccrs.PlateCarree())#Destino
ax1.plot(-78.489166,-0.211203,label='RX/POS2',markersize=20,markeredgewidth=1.0, marker='*', linestyle='', color='gold', transform=ccrs.PlateCarree())#Destino


#ax1.gridlines(draw_labels=True, dms=True, x_inline=False, y_inline=False)
ax1.legend(loc="upper right", fontsize=15,bbox_to_anchor=[1, 1],ncol=3, shadow=True, title=" ", fancybox=True)# El bbox_to_anchor es un punto de referencia alrededor del                                                                                           # cual se ubica la caja de leyendas segun lo diga el loc

''''
init_point=[RX_pos[1],RX_pos[0]]#[Longitud,Latitud]
end_point=[TX_pos[1],TX_pos[0]]#[Longitud,Latitud]
Dist_cartopy,azimuth1,azimuth2=np.array(geod.inverse(init_point,end_point).T)#Distancia con Cartopy
Dist_vincenty=distance_between(init_point,end_point)
Azimuth_forward=course_to(init_point,end_point)
valor_f1=float(Dist_cartopy)
valor_f2=float(Dist_vincenty)
valor_f3=float(azimuth1)
valor_f4=float(Azimuth_forward)
str_dist1=str(round(valor_f1,3))+'m'
str_dist2=str(round(valor_f2,3))+'m'
str_az1=str(round(valor_f3,3))+'°'
str_az2=str(round(valor_f4,3))+'°'
fig.text(0.01,0.80,'Cálculo de distancia(RX_pos1)',fontsize=14,fontweight='bold')
fig.text(0.015,0.77,'Cartopy:'+str_dist1,fontsize=14,style='italic')
fig.text(0.015,0.74,'Vicenty:'+str_dist2,fontsize=14,style='italic')
fig.text(0.01,0.70,'Cálculo de azimuth(RX_pos1)',fontsize=14,fontweight='bold')
fig.text(0.015,0.67,'Cartopy:'+str_az1,fontsize=14,style='italic')
fig.text(0.015,0.64,'Forward azimuth:'+str_az2,fontsize=14,style='italic')
'''
print('Distancia con el microcontrolador:'+str(Dist_vincenty)+'m')
print('Distancia con cartopy:'+str(float(Dist_cartopy))+'m')
print('Azimuth forward con el microcontrolador:'+str(Azimuth_forward)+'°')
print('Azimuth forward con cartopy:'+str(float(azimuth1))+'°')


plt.show() # show the plot
