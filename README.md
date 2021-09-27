# Summary

PMACCT's pre-tagging function can be used to enrich the data stream:

In the use case below when the IP address \<ip\> is matched a new label is generated & added to the data stream. 
The final label is going to be a string composed by the two fields specified within the \<set\_label\> statements. 
Comma (",") is used as a field separator: "node\_id\_1,platform\_id\_1"

```text
set_label=<node_id_1>        ip=<specific_ip>    label=<node_id>  jeq=<platform_id>
set_label=<platform_id_1>    label=<platform_id>
```
