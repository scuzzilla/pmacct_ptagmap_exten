### Summary

PMACCT's pre-tagging function can be used to enrich the data stream:

In the use case below when the IP address \<ip\> is matched a new label is generated & added to the data stream. 
The final label is going to be a string composed by the N fields specified within the \<set\_label\> statements. 
- Comma (",") is currently used as a field separator between the two (or more) lables: "\<key1,value1\>,\<key2,value2\>, ... , 
\<keyN, valueN\>".
- Each sub-label is composed of two fields (key and value) disposed in a strict order, first key second label,  and 
separated by a delimiter: "\<key,value\>" 

```text
set_label=<key1-value1,key2-value2,...,keyN-valueN>   ip=<specific_ip_1>
set_label=<key1-value1,key2-value2,...,keyN-valueN>   ip=<specific_ip_2>
...
set_label=<key1-value1,key2-value2,...,keyN-valueN>   ip=<specific_ip_N>
!
!default label   
set_label=<key1-value1,key2-value2,...,keyN-valueN>
```

More about PMACCT's pre-tagging can be found [here](https://github.com/scuzzilla/pmacct/blob/master/CONFIG-KEYS).


### Current JSON output

The way pre-tagging is currently developed doesn't easily allow to load the "multi-labels" string in a 
database without a sort of post-processing job in charge of manipulating the string itself (like splitting the string
into two sub-strings).

Here below is an abstraction of the current data-stream JSON output.

```JSON
{
  "label": "key1,value1,key2,value2,...,keyN,valueN",
  "other1": "other1",
  "other2": "other2",
  "other3": "other3"
}
```

### Future JSON output

The main goal of extending the PMACCT's pre-tagging functionality is to avoid any kind of post-processing job on the 
produced "multi-labels" string.

Here below is an abstraction of the future data-stream JSON ouput.

```JSON
{
  "label": {
    "key1": "value1",
    "key2": "value2"
  },
  "other1": "other1",
  "other2": "other2",
  "other3": "other3"
}
```

### Current AVRO schema

Here below is reported the current AVRO schema used to define the label field (type string). 

```JSON
{                                                                                                                                                                                                                                                                                                                             
  "type": "record",                                                                                                                                                                                                                                                                                                           
  "name": "acct_data",                                                                                                                                                                                                                                                                                                        
  "fields": [                                                                                                                                                                                                                                                                                                                 
    {                                                                                                                                                                                                                                                                                                                         
      "name": "label",                                                                                                                                                                                                                                                                                                        
      "type": {                                                                                                                                                                                                                                                                                                               
        "type": "string"                                                                                                                                                                                                                                                                                                      
      }                                                                                                                                                                                                                                                                                                                       
    }
  ]
}
```

### Future AVRO schema

Here below is reported the future AVRO schema which will be used to define the new label field (type map fo strings).

```JSON
{
  "type": "record",
  "name": "acct_data",
  "fields": [
    {
      "name": "label",
      "type": {
        "type": "map",
        "values": {
          "type": "string"
        }
      }
    }
  ]
}
```
