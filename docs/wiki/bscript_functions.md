# Built-in Functions

The following functions are built-in and available globally to all **bScript** scripts:

## Output and Logging

| Name     | Parameters       | Return | Description                                                                                 |
|----------|------------------|--------|---------------------------------------------------------------------------------------------|
| print    | `values`(s): Any | Void   | Prints out all passed parameters to the console. Use log functions for non-temporary prints |
| logdebug | `value`(s): Any  | Void   | Logs the passed values to the logging system at DEBUG level                                 |
| loginfo  | `value`(s): Any  | Void   | Logs the passed values to the logging system at INFO level                                  |
| logerror | `value`(s): Any  | Void   | Logs the passed values to the logging system at ERROR level                                 |

## Math

| Name   | Parameters                     | Return  | Description                                                                                                    |
|--------|--------------------------------|---------|----------------------------------------------------------------------------------------------------------------|
| abs    | `n`: Numeric                   | Numeric | Returns the absolute value of n                                                                                |
| sqrt   | `n`: Numeric                   | Numeric | Returns the square root of n                                                                                   |
| round  | `n`: Numeric                   | Numeric | Returns n rounded to the nearest whole number                                                                  |
| ceil   | `n`: Numeric                   | Numeric | Returns n rounded towards positive infinity                                                                    |
| floor  | `n`: Numeric                   | Numeric | Returns n rounded towards negative infinity                                                                    |
| min    | `value`(s): Numeric/Array      | Numeric | Returns the minimum value from the passed values. If an array is passed returns the minimum value in the array |
| max    | `value`(s): Numeric/Array      | Numeric | Returns the maximum value from the passed values. If an array is passed returns the maximum value in the array |
| sum    | `value`(s): Numeric/Array      | Numeric | Returns the sum of the passed values. If an array is passed, returns the sum of its elements                   |
| random | `min`: Numeric, `max`: Numeric | Numeric | Returns a random number in the range [min, max]                                                                |
| cos    | `n`: Numeric (degrees)         | Numeric | Returns the cosine of n                                                                                        |
| sin    | `n`: Numeric (degrees)         | Numeric | Returns the sine of n                                                                                          |
| tan    | `n`: Numeric (degrees)         | Numeric | Returns the tangent of n                                                                                       |
| atan2  | `y`: Numeric, `x`: Numeric     | Numeric | Returns the angle to (x, y) in degrees                                                                         |

## Type Conversion

| Name | Parameters   | Return  | Description                    |
|------|--------------|---------|--------------------------------|
| num  | `s`: String  | Numeric | Converts s to a number         |
| str  | `value`: Any | String  | Converts the value to a string |

## Control Flow

| Name  | Parameters                           | Return  | Description                                                                                                                                   |
|-------|--------------------------------------|---------|-----------------------------------------------------------------------------------------------------------------------------------------------|
| time  | N/A                                  | Numeric | Returns a number of milliseconds elapsed. Take two timestamps and the difference is time elapsed                                              |
| sleep | `ms`: Numeric                        | Void    | Pauses script execution for the given number of milliseconds                                                                                  |
| run   | `script`: String, `concurrent`: Bool | Bool    | Runs the given script (may be file or code), optionally in a new thread. Returns the return value of the script, or just `true` if concurrent |
| error | `message`: String                    | N/A     | Terminates the script with the given error message                                                                                            |
| exit  | N/A                                  | N/A     | Terminates the script with no error                                                                                                           |
