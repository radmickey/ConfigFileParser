# ConfigFileParser

## Format

 A config file is a text file in ASCII encoding. The following is a specification of the format.
 The primary construct for a config file is a KeyValue pair.
 The format is case-sensitive. Spaces and blank lines are ignored.

### Key\Value

 The key is on the left, followed by an equal sign, followed by the value.
````
key = "value"
````

 Both the key and value are required. Line breaks are not allowed.
 The value cannot be redefined for the same key (within the same section).

### Key

The key may consist of:
`````
 - Uppercase and lowercase Latin letters
 - Digits
 - Symbols '-' and '_'
`````
#### The key cannot be of zero length.
```
number = 0
name = "M3100"
```

#### Value

 The value can be one of the following types
```
- Integer number
- Floating-point number
- String
- Boolean variable
- Array of values
```

## Integer
#### Consists of digits (one or more). A '+' or '-' sign may be added as the first character.
```
key1 = 1
key2 = -2022
key3 = +128
```

#### The possible value will fit in an int32_t.

## Floating-point number

#### Consists of digits (one or more) and a single period. There must be at least one digit before and after the period.
##### A '+' or '-' sign may be added as the first character.
```
key1 = 1.23
key2 = -2.77
key3 = -0.0001
```

## String

#### The string is surrounded by double quotes. It can contain any characters.
```
key1 = "value"
key2 = "Hello world!"
```

## Boolean value

#### The literal "true" or "false" is used for boolean values.
```
key1 = true
key2 = false
```

## Array

#### The array is surrounded by square brackets []. Elements are separated by commas.
#### The array can consist of any correct values, not necessarily of the same type.
````
key1 = [1, 2, 3, 4, 5]
key2 = ["Hello", "world"]
key3 = [[1, 2, 3, 4, 5], ["Hello", "world"]]
key4 = [1, 3.3, "ITMO", [true, false]]
````
## Sections

#### In addition to the key-value block, the format supports sections. Sections allow sets of key-value pairs to be combined into logical blocks.
#### A section is defined as a key surrounded by square brackets [].
````
[name]
`````

#### After a section declaration, all subsequent key-value pairs belong to that section until the next one is declared.
````
[section-1]
key1 = 1
key2 = "2"

[section-2]
key1 = 2
````

#### Although a section follows the key rules, it can also contain the symbol '.'. This determines nested sections.
````
[section-1.part-1.x]

[section-1.part-1.y]

[section-1.part-2.x]

[section-2.z]
````

#### Thus, a section can contain both key-value pairs and other sections. The key and sub-section names cannot match.
````
[A]
key1 = 1

[A.B]
key2 = 3

[A.B.C]
key3 = 3
````
## Comments

#### The config file may contain single-line comments. Comments start with the '#' symbol, except when '#' is inside a string value.
````
key1 = "value1"  # some comment
# another comment
````
