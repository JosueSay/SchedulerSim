# Recursos / Resources

## Formato del Archivo

El archivo de entrada contiene una lista de recursos, uno por línea, con el siguiente formato:

```bash
<NOMBRE RECURSO>, <CONTADOR>
```

Donde:

* **NOMBRE RECURSO**: Identificador único del recurso (cadena de texto).
* **CONTADOR**: Número entero que representa la cantidad o estado del recurso (entero).

### Ejemplo de línea válida

```bash
R1, 1
```

## Estructura Interna del Recurso

Cada línea del archivo es convertida a una estructura de datos interna llamada `Resource` que contiene al menos los siguientes atributos:

| Atributo   | Tipo    | Descripción                                                                      |
| ---------- | ------- | -------------------------------------------------------------------------------- |
| `name`     | String  | Identificador único del recurso.                                                 |
| `counter`  | Integer | Contador o cantidad del recurso.                                                 |
| `isLocked` | Integer | Indicador de estado: 0 para libre, 1 para bloqueado (usado para sincronización). |

> Nota: La estructura `Resource` puede contener otros atributos adicionales según la implementación, pero estos tres son los mínimos requeridos y se obtienen directamente del archivo de entrada.

## Detalles y funcionalidades implementadas

### Estructura `Resource`

* Representa un recurso disponible para los procesos en la simulación.
* Contiene los siguientes atributos:

  * `name`: Identificador único del recurso (string).
  * `counter`: Número que indica la cantidad o disponibilidad del recurso.
  * `isLocked`: Estado de bloqueo para sincronización, donde:

    * `0` indica que el recurso está libre.
    * `1` indica que el recurso está bloqueado.
