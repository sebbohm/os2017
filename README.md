
# os2017
BIC Sommer Semester 2017 Betriebssysteme

Übernommen von Fmyschkin/os2017 


Kurze Anleitung zur Erstellung von Make und Doc

1) Make:
Vorlage von fgrep genommen
Beim Punkt "variables" die Compiler Informationen eingetragen, die definiert waren
Beim Punkt "targets" myfind eingesetzt wo mygrep verwendet wurde
Alle Relationen zu Headerfiles gelöscht

Ausführung von Make im Directory des Source-Codes und des Makefiles mittels: make myfind

2) Doc
Vorlage von fgrep genommen
Im File doxygen.dcf alle fgreps mit myfind ausgetauscht.

Ausführung folgenden Befehals im Directory des Source-Codes, des Makefiles und des doxygen.dcf: make doc 

test