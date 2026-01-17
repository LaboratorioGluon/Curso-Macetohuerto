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

