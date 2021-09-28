### Summary

PMACCT's pre-tagging function can be used to enrich the data stream:

In the use case below when the IP address \<ip\> is matched a new label is generated & added to the data stream. 
The final label is going to be a string composed by the two fields specified within the \<set\_label\> statements. 
- Comma (",") is currently used as a field separator between the two lables: "\<key1,value1\>,\<key2,value2\>".
- Each label is composed of two fields (key and value) disposed in a strict order and separated by a delimiter: 
"\<key,value\>" 

```text
set_label=<key1,value1>    ip=<specific_ip>     label=<node_id>  jeq=<platform_id>
set_label=<key2,value2>    label=<platform_id>
```

More about PMACCT's pre-tagging can be found [here](https://github.com/scuzzilla/pmacct/blob/master/CONFIG-KEYS).


### Current JSON output

The way pre-tagging is currently developed doesn't easily allow to load the "multi-labels" string in a 
database without a sort of post-processing job in charge of manipulating the string itself (like splitting the string
into two sub-strings).

Here below is an abstraction of the current data-stream JSON output.

```JSON
{
  "label": "key1,value1,key2,value2",
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
