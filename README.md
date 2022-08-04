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
        <a href="https://processing.org/" target="_blank">
          <!-- https://app.zipchi.ir/2020/08/Processing_cover_190896269746.jpg-->
          <img width="222px" src="https://upload.wikimedia.org/wikipedia/commons/2/2e/Processing_3_logo.png">
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

  <!-- Social Network -->
<h1 align="center">
<a href="https://www.instagram.com/lunox.code/">
  <img align="center" 
       alt="Lunox's Instagram" 
       width="22px" 
       src="https://user-images.githubusercontent.com/55005374/103146167-0b04ac00-470b-11eb-84fc-db4b7299e4ef.png" />
  </a>
  
<a href="https://www.linkedin.com/in/lunox/">
  <img align="center" 
       alt="Linkdein" 
       width="22px" 
       src="https://user-images.githubusercontent.com/55005374/103146171-312a4c00-470b-11eb-8839-992580bb8206.png" />
  </a>

  <a href="https://www.codegrepper.com/app/profile.php?id=79189">
  <img align="center" 
       alt="Grepper" 
       width="22px" 
       src="https://user-images.githubusercontent.com/55005374/103146498-0b537600-4710-11eb-949e-bff2c2ab7391.png" />
  </a>
  
<a href="https://stackoverflow.com/users/12381868/lunox?tab=profile">
  <img align="center" 
       alt="Stack Overflow" 
       width="22px" 
       src="https://user-images.githubusercontent.com/55005374/103146236-e52bd700-470b-11eb-861e-e6f549b02b88.png" />
  </a>
  
<a href="mailto:lunox.code@gmail.com">
  <img align="center" 
       alt="Gmail" 
       width="22px" 
       src="https://user-images.githubusercontent.com/55005374/103146250-0d1b3a80-470c-11eb-8ead-a92232d45d6e.png" />
  </a>
</h1>




<!-- Background -->

<!-- I do add this "&nbsp;" because I can't center the GIFT, let me know if you know how do it -->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
![ezgif com-gif-maker](https://user-images.githubusercontent.com/55005374/95673501-37764680-0b66-11eb-8ee1-d4f4a2b285d9.gif)

&nbsp;

<!-- Technical Skills -->
<p><H3 align="center"><strong> Technical Skills: 💻 </strong></p>
  
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/101125531-160e6580-35bf-11eb-8256-f599b154f3ee.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/103146278-8ca90980-470c-11eb-90bd-8e20951c9146.png"></code>
  <code><img height="40" src="https://raw.githubusercontent.com/devicons/devicon/master/icons/java/java-original-wordmark.svg"></code>
  <code><img height="40" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/python/python.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/103146298-d98ce000-470c-11eb-973d-3ff9e1b90561.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/103146335-3d170d80-470d-11eb-9fce-ff775c77b96b.png"></code>
  <code><img height="40" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/terminal/terminal.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/103146218-b57ccf00-470b-11eb-8fcc-aa46cab9253f.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95688411-345f7280-0bc7-11eb-9513-82e0452a81eb.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/100307358-3c068b00-2f6b-11eb-9f07-e262ad248471.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95686171-87cac400-0bb9-11eb-9d49-390f3543a0a6.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95686553-d4170380-0bbb-11eb-94f2-c528413c7bad.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95686705-d9c11900-0bbc-11eb-87f5-a149b86cde5a.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95686779-5fdd5f80-0bbd-11eb-9a0b-8eb90d565518.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95687393-a2546b80-0bc0-11eb-8991-c0c72326f29c.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95687569-95844780-0bc1-11eb-8bdb-93fb29d10602.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95687639-117e8f80-0bc2-11eb-8aea-f236a8eca50b.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95687670-51de0d80-0bc2-11eb-826b-83fb8c5ec221.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/100187906-b7eecd80-2eae-11eb-8074-b65db8dfaecb.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95687701-80f47f00-0bc2-11eb-89f5-a1a8e6788aeb.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95688226-c6ff1200-0bc5-11eb-82cc-33e35bcb0910.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95688875-5dcdcd80-0bca-11eb-8915-b3cf9791ca3c.png"></code>

  </p>
  
&nbsp;  

  <!-- Skills to learn -->
<p><H3 align="center"><strong>Skills to learn: 🌐</strong></p>
  
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/101125928-05122400-35c0-11eb-836b-4c2e4de16070.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/99864609-ecd6e980-2b69-11eb-8268-1a455c00eefe.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/99864949-fbbe9b80-2b6b-11eb-8b5a-4ca8cd68261e.png"></code>  
  <code><img height="40" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/css/css.png"></code> 
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95688807-0d567000-0bca-11eb-8cec-9a813166d3d8.png"></code>
  <code><img height="40" src="https://user-images.githubusercontent.com/55005374/95686219-bd6fad00-0bb9-11eb-9dfd-be7dd980d005.png"></code>
  
  </p>
&nbsp;

<!-- GitHub Stats -->
<H2 align="center"><strong>GitHub Statistics: 📈
  </strong>
</H2>
    <p align="center">
      <div align="center">
    </p>
    
<a href="https://github.com/Lunox-code?tab=repositories">
  <img align="center" 
       src="https://github-readme-stats.vercel.app/api/top-langs/?username=Lunox-code&layout=compact&show_icons=true&title_color=81a1c0&icon_color=79ff97&text_color=d5dbe6&bg_color=2e3440" 
       alt='Lunox's favorite languages" />
</a>
  
<a href="https://github.com/Lunox-code">
  <img align="center"
       src="https://github-readme-stats.vercel.app/api?username=Lunox-code&show_icons=true&hide=contribs,prs&cache_seconds=86400&theme=nord" />
</a>
