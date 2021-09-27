# Summary

PMACCT's pre-tagging function can be used to enrich the stream:

In the use case below the IP address \<ip\> is matched and a new label is generated and added to the data stream. 
The final label is going to be a string composed by the two fields specified within the \<set\_label\> statements. 
Comma (",") is used as a field separator.

```text
set_label=<node_id>        ip=<specific_ip>    label=<node_id>  jeq=<platform_id>
set_label=<platform_id>    label=<platform_id>
```
