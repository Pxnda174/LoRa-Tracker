# LoRa-Tracker
En esta carpeta se encuentran los archivos de software y hardware desarrollados para el trabajo de integración curricular/periodo 2022-A
<!-- Title -->
<h1 align="center">DISEÑO E IMPLEMENTACIÓN DE UN PROTOTIPO DE SISTEMA DE RASTREO DE DRONES BASADO EN TECNOLOGÍA LORA Y HERRAMIENTAS OPEN SOURCE
  <h1 align="center">Facultad de Ingeniería Eléctrica y Electrónica
  </h2></h1>
  
<p align="center"><a target="_black" rel="noopener noreferrer"><img width="500" src="https://servicios-it.epn.edu.ec/images/dgip/logo-2.png" alt="EPN logo"></a></p>

Para conocer más sobre la institución, por favor visite los siguientes enlaces:

- [Escuela Politécnica Nacional(EPN)](https://www.epn.edu.ec/)
- [Departamento de Automatización y Control Industrial (DACI)](https://daci.epn.edu.ec/)

Software de desarrollo OPEN-SOURSE usado:
<p align="center"><a target="_black" rel="noopener noreferrer"><img width="100" src="https://cdn.worldvectorlogo.com/logos/open-source-hardware.svg" alt="DIY logo"></a></p>
 
 <table>
  <tbody>
    <tr>
      <td align="center" valign="middle">
        <a href="https://www.arduino.cc/">
          <img width="180px" src="https://upload.wikimedia.org/wikipedia/commons/8/87/Arduino_Logo.svg">
        </a>
      </td>
      <td align="center" valign="middle">
        <a href="https://code.visualstudio.com/" >
          <!--logo:https://anthoncode.com/wp-content/uploads/2019/01/visual-studio-2012-logo-png.png -->
          <img width="250px" src="https://i.pinimg.com/originals/12/b2/a8/12b2a82ff9564d092ebdf53a8432ef51.png">
        </a>
      </td>
      <td align="center" valign="middle">
        <a href="https://www.python.org/" target="_blank">
          <img width="222px" src="https://brandslogos.com/wp-content/uploads/thumbs/python-logo-vector-1.svg">
        </a>
      </td>      
      <td align="center" valign="middle">
        <a href="https://www.espressif.com/en/products/socs/esp32" target="_blank">
          <!-- https://app.zipchi.ir/2020/08/Processing_cover_190896269746.jpg-->
          <img width="222px" src="https://www.espressif.com/sites/all/themes/espressif/logo-black.svg">
        </a>
      </td>
    </tr><tr></tr>
  </tbody>
</table>
 
 
 <h2 align="center"> Para el cálculo de la distancia entre dos puntos a partir de los datos de posición GPS, se lo realiza empleando
 un caso especial de la fórmula de Vincenty:</h2>
 
 ->Ecuación para determinar la distancia entre dos puntos:

<p align="center"><a target="_black" rel="noopener noreferrer"><img width="1500" src="https://latex.codecogs.com/svg.latex?\large&space;{\color{White}&space;\Delta&space;\sigma=&space;\arctan&space;\left(\frac{\sqrt{&space;\left(\cos(\phi_{2})\cdot&space;\sin(\Delta&space;\lambda)&space;\right)^2&plus;\left(\cos(\phi_{1})\cdot&space;\sin(\phi_{2})-\sin(\phi_{1})\cdot&space;\cos(\phi_{2})\cdot&space;\cos(\Delta&space;\lambda)\right)^2}}{\sin(\phi_{1})\cdot&space;\sin(\phi_{2})&plus;\cos(\phi_{1})\cdot&space;\cos(\phi_{2})\cdot&space;cos(\Delta&space;\lambda)}\right)}" title="\large {\color{Blue} \Delta \sigma= \arctan \left(\frac{\sqrt{ \left(\cos(\phi_{2})\cdot \sin(\Delta \lambda) \right)^2+\left(\cos(\phi_{1})\cdot \sin(\phi_{2})-\sin(\phi_{1})\cdot \cos(\phi_{2})\cdot \cos(\Delta \lambda)\right)^2}}{\sin(\phi_{1})\cdot \sin(\phi_{2})+\cos(\phi_{1})\cdot \cos(\phi_{2})\cdot \cos(\Delta \lambda)}\right)}" /></a></p>


<h2 align="center"> Para detérminar el rumbo a seguir entre dos puntos, se utiliza la fórmula que es conocida como " forward azimuth" la cual, si se sigue en línea recta a lo largo de un arco de círculo máximo, lo llevará desde el punto de inicio hasta el punto final:</h2>
 
 ->Ecuación para determinar el rumbo entre dos puntos:
 
 <p align="center"><a target="_black" rel="noopener noreferrer"><img width="1000" src="https://latex.codecogs.com/svg.latex?\large&space;{\color{White}&space;\theta=\arctan&space;\left(\frac{\sin(\Delta&space;\lambda)\cdot&space;\cos(\phi_{2})}{\cos(\phi_{1})\cdot&space;\sin(\phi_{2})-\sin(\phi_{1})\cdot&space;\cos(\phi_{2})\cdot&space;\cos(\Delta&space;\lambda)&space;}&space;\right)&space;}" title="\large {\color{Blue} \theta=\arctan \left(\frac{\sin(\Delta \lambda)\cdot \cos(\phi_{2})}{\cos(\phi_{1})\cdot \sin(\phi_{2})-\sin(\phi_{1})\cdot \cos(\phi_{2})\cdot \cos(\Delta \lambda) } \right) }" /></a></p>

<!--Diagrama de flujo del sistema receptor-->
 <h2 align="center"> Diagrama de flujo del sistema receptor:</h2>
<img src="https://github.com/Pxnda174/Tesis_GPS_EPN/blob/main/Imagenes/Receptor_LoRa_ESP32.svg" width="3000" align="center"/>

<!-- GitHub Stats -->
<H2 align="center"><strong>GitHub Statistics: 📈
  </strong>
</H2>
    <p align="center">
      <div align="center">
    </p>
    
<a href="https://github.com/Pxnda174?tab=repositories">
  <img align="center" 
       src="https://github-readme-stats.vercel.app/api/top-langs/?username=Lunox-code&layout=compact&show_icons=true&title_color=81a1c0&icon_color=79ff97&text_color=d5dbe6&bg_color=2e3440" 
       alt='Lunox's favorite languages" />
</a>
  
<a href="https://github.com/Pxnda174">
  <img align="center"
       src="https://github-readme-stats.vercel.app/api?username=Lunox-code&show_icons=true&hide=contribs,prs&cache_seconds=86400&theme=nord" />
</a>
