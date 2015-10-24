.SK
.nr Hc 1
.HU "Appendix B \(em Guidelines for Using G2"
.nr Hc 0
.ft I
.ce
J.E. Weythman
.ft R
.SP 2
.ix "%begin G2
The G2 data language was designed to provide an efficient 
but change-tolerant coupling between compiled programs. 
With care, a fair amount of inconsistency can be tolerated 
among a set of communicating processes. This should allow
program developers to be more effective in a less than 
consistent environment.
.P
When a record is passed between programs using G2 some 
useful interpretations take place:
.AL
.LI
Only non-empty fields and their attendant hierarchy are 
transmitted; non-transmitted fields are seen by the receiver 
as empty strings. This provides a form of data compression 
while relieving the transmitter of having to meet the
expectations of the receiver(s).
.LI
Receivers discard unknown fields. This means that a 
transmitter may enrich a G2 record with out affecting its 
receivers. The receivers may upgrade to the enriched records 
as convenient.
.LI
Receivers truncate lengthy fields. This preserves the 
integrity of C data structures.
.LI
Transmitters send only printable (\f(CWisprint(3)\fR) data. 
Receivers accept only printable data. Strings are terminated 
by the first non-printable byte. Thus, the communications 
link is filtered at both ends.
.LE
.P
With the above mechanisms in mind here are some serviceable 
.ix "G2~guidelines~for~using
guidelines for using G2 records.
.AL
.LI
Be generous in defining the maximum length of G2 fields. 
Since only non-empty strings are transmitted, the cost of 
defining larger G2 records is mainly in the size of the C 
data structure allocated within the program. 
These buffers are usually few in number, so one can 
afford to relax a bit 
about their size. It is better to have a larger G2 
definition than needed than to ``creep'' the definition. 
For example, one should specify a numeric field at least 11 
bytes long (long enough to represent, in ASCII, the biggest 
negative integer on a 32 bit machine).
.LI
Be miserly in defining G2 names; use the structuring 
facilities of G2 to create scopes where short names are 
distinctive. The root name of a G2 record becomes an 
external symbol so it should be kept to 6 characters. 
Also, since most UNIX output devices have tabs set every 
eight spaces, G2 records are easier to read raw when field 
names are less than 7 characters long.  
.LI
Never change the meaning of a name; rather, invent a new one 
and gracefully retire the old one. This may mean supporting 
redundant data fields until other programs in a system have 
time to conform to new schemes.
.LE
.ix "%end G2
.sE
