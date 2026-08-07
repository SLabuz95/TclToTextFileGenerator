# TclTranspiller

## Introduction
The main goal is to generate modified text files based on data contained
in provided Tcl script files.
It's GUI application based on Qt libraries.
The application is a part of Master's Thesis.

### General
The main task of the application is to generate text based on user-defined
generation rules and data contained in files or directories provided by the user.\
The application allows to:
- manage projects and a generation configuration and generation rules associated 
with the project,
- generate text files based on other files and directories,
- generate text based on provided text.

### Main use case
To generate a user needs to create a project called the generation instance.
Each project contains a local generation configuration. The user can modify it with a configuration panel.\
Additonally the user needs to add a files interpreter instance or a text interpreter instance to the project.
- With the files interpreter the user can analyse and generate groups of files and directories.
The result of the generation from the instance is an output directory with the same structure as provided files and directories.
- With the text interpreter the user can analyse any text input. The application displays the generated text for the user.
The main goal of the instance is to generate small chunks of data. Additonally it allows the user to test and improve the generation configuration.

Each generation process creates the report with errors and the report with exceptions.
The exception is TCL procedure call, which:
- failed to satify conditions of their rules,
- there are not rules defined for the procedure.

### Generation configuration
The configuration contains a few sections:
1. Atributes - contains key - value's list, which defines variables to be used by generation rules while generation process,
2. Filtration mode - contains list of procedure names, which will generate the output text if the generation process is started in filtration mode.
The other procedure names, which are not defined in the list, will not generate the output text in the filtration mode.
3. Phases of content generator - contains list of user defined phases of the content generator. 
The phase contains user-defined rules, which are evaluted by the content generator.
The phase is like a evalution context of the content generator, which means that for each new line of text data
the content generator evalutes the rules from the current phase. The user can change the phase dynamically with the rules.
4. Defined Tcl procedures - list of defined Tcl procedures for Tcl generator. 
Defined Tcl procedure contains sets of user-defined rules directly associated with procedure name provided by the user.
The rules are evaluted by the Tcl generator. 
Each set is associated with:
- end of procedure evaluation event,
- end of procedure's specific argument evaluation event of specified index number,
- end of procedure's default argument evaluation event of any index number, which is not defined as specific

5. Undefined Tcl procedures - the section is very similar to defined Tcl procedure, but procedure name is not defined by user.
The sets for the undefined Tcl procedures are evaluated for each Tcl procedure, which is not defined in
Defined Tcl procedures section.


### Content generator and Tcl generator
The generation process reads provided input data line by line.
For each line, the process evalutes rules with the content generator.
To evaluate the line as Tcl script use "TclParse" action of the content generator to parse the line with rules for Tcl generator.
The Tcl generator keeps it state for the time of generation process evaluation.



