# Acciones / Actions

## Formato del Archivo

El archivo de entrada contiene una lista de acciones realizadas por procesos sobre recursos, una acción por línea, con el siguiente formato:

```bash
<PID>, <ACCION>, <RECURSO>, <CICLO>
```

Donde:

* **PID**: Identificador del proceso que realiza la acción (cadena de texto).
* **ACCION**: Tipo de acción que realiza el proceso sobre el recurso. Puede ser:

  * `READ` (lectura)
  * `WRITE` (escritura)
* **RECURSO**: Nombre del recurso involucrado (cadena de texto).
* **CICLO**: Ciclo de simulación en que se realiza la acción (entero).

### Ejemplo de línea válida

```bash
P1, READ, R1, 0
```

## Estructura Interna de la Acción

Cada línea del archivo se convierte en una estructura interna llamada `Action` con los siguientes atributos:

| Atributo       | Tipo    | Descripción                                    |
| -------------- | ------- | ---------------------------------------------- |
| `pid`          | String  | Identificador único del proceso.               |
| `action`       | Enum    | Tipo de acción: `ACTION_READ` o `ACTION_WRITE` |
| `resourceName` | String  | Nombre del recurso sobre el que se actúa.      |
| `cycle`        | Integer | Ciclo de simulación en que ocurre la acción.   |

> Nota: El tipo `ActionType` es un enumerado que contiene los valores posibles de acción (`READ` y `WRITE`).

## Detalles y funcionalidades implementadas

### Estructura `Action`

* Representa una acción específica que un proceso realiza sobre un recurso en un ciclo de simulación determinado.
* Contiene los siguientes atributos esenciales:

  * `pid`: Identificador del proceso (cadena de texto).
  * `action`: Tipo de acción (lectura o escritura).
  * `resourceName`: Nombre del recurso involucrado.
  * `cycle`: Momento en el tiempo (ciclo) en que ocurre la acción.

### Enumeración `ActionType`

| Valor          | Descripción                        |
| -------------- | ---------------------------------- |
| `ACTION_READ`  | Acción de lectura sobre recurso.   |
| `ACTION_WRITE` | Acción de escritura sobre recurso. |
