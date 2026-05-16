# Curso Online Maker III: :seedling: Macetohuerto :seedling:

> [!NOTE]
> Este es el repositorio donde vamos guardando el código que realizamos para el curso en directo que estamos haciendo en mi [Canel de Twitch](https://www.twitch.tv/labgluon) y [Canal de Youtube](https://www.youtube.com/laboratoriogluon). 


Estamos haciendo un MacetoHuerto! Un pequeño dispositivo para controlar el riego y el estado de las plantas que podemos tener en un piso, de ahí el "maceto" 😁. El plan es hacer que el macetohuerto mida: la humedad del suelo, la temperatura, presión y humedad del ambiente; La cantidad de luz que llega a la planta y envíe (MQTT) estos datos a un servidor donde se almacenará (InfluxDB) y se podrán analizar (Grafana). Por otro lado, queremos que todo el dispositivo esté alimentado por paneles solares, y él mismo gestione la batería.

Por lo tanto, en este curso vamos a aprender a:

- :hammer_and_pick: Hardware:
    - **BME280**: Implementar el interfaz para la librería de Bosch.
    - **ADS1115**: Crear la librería del ADC para leer los sensores.
    - **LDR** y **Humedad**: Conectar y entender como leer los sensores.
    - **MOSFETS**: Control de motor para riego. 
    - **Hx711** y **Célula de carga**: Para medir el peso del agua del depósito.
    - **Paneles Solares** y **Baterías**: Cómo gestionar la carga de baterías mediante panel solar y balanceadores de carga.
- :computer: ESP32:
    - Usar el **I2C** para hablar con los sensores.
    - Enviar los datos por **Wifi** usando **MQTT**.
    - Aprender de los modos de **bajo consumo**.
    - Usar **FreeRTOS** y **multihilo**.
- :desktop_computer: Servidor:
    - Configurar **proxmox**
    - Instalar y configurar **Telegraf** + **InfluxDB** + **Grafana**
    - Recibir y mostrar datos desde **MQTT**

## Diseño Objetivo:

![Diseño a del MacetoHuerto](/doc/esquemaCompleto.png)

## Sensores

### Peso: HX711 + Galga de 1Kg
#### Calibración

Para la calibración del hx711 hacen falta dos puntos, al ser un sensor líneal vamos a calibrar una recta. Es decir, este método no funcionaría
con sensores no lineales. 

Lo primero es hacer el `tare` o poner a cero y luego hacer la regresión lineal. La regresión líneal tiene la forma `y-y0 = (x-x0) · (y1-y0)/(x1-x0)`. Los puntos `(x0, y0)`, `(x1, y1)` son los datos de calibración que tenemos nosotros. En este paso es importante saber que vamos a poner en la `x` y qué en la `y`. Nosotros vamos a tener la entrada del hx711 y de esos datos queremos sacar el peso en gramos, por lo tanto nuestra `x` será la salida en bruto del hx711 y la variable dependiente `y` el peso real que hay en la báscula.

![Diseño a del MacetoHuerto](/doc/regresionLineal.png)

Por lo tanto el proceso será:

1. Dejas la báscula vacía
2. Apuntas el dato y lo guardas como "tara" usando la función `hx711_setTare`.
3. Pones un peso pequeño y guardas el dato. Por ejemplo: peso 100g y el hx711 me da 40000, sería (40000, 100)
3. Lo mismo con un peso grande. (250000, 1000)
4. Calculamos la regresión lineal usando la formula de arriba.
    1. La pendiente es: `p = (y1-y0)/(x1-x0)` = 900/210000 = 4.29e-3
    2. Despejamos la y: `y =  (x-x0)·p + y0 = x·p + y0 - x0·p = x·4.29e-3 - 71.6`.
    3. Lo que va con la x es la _gain_ (4.29e-3) y el resto es el offset (-71.6)
5. Guardamos los datos de calibración del hx711: `hx711_setScaleOffset(dev, 4.29e-3, -71.6);`