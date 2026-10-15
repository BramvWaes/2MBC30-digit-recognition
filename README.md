# Recognizing Digits By Shape
This repository contains the code used to recognize digits by their shape. The repository is divided into a few sections:
* `classes` contains the dataclasses used for the analysis: nodes for the graph, strokes for the paths traced through that graph and patterns for the structures of the numbers
* `data` contains `.csv` files extracted from `digitsimagesandlabels.mat`
* `modules` contains the logic modules used in the process
* `patterns` uses the aforementioned `pattern` class to define behaviour per digit

## Output
The analysis is deterministic, meaning the output should be the same every time. Running the program resulted in the following output, which can be verified by ...
```shellsession
=== test set (2007 images) ===

digit  images  correct  wrong  none   recall  precision
    0     359      308      6    45    85.8%      84.6%
    1     264       99      2   163    37.5%      93.4%
    2     198       25     20   153    12.6%     100.0%
    3     166       90      6    70    54.2%      67.2%
    4     200       12     22   166     6.0%      92.3%
    5     160       26      7   127    16.2%     100.0%
    6     170       50     14   106    29.4%      96.2%
    7     147       26     13   108    17.7%     100.0%
    8     166       47     37    82    28.3%      75.8%
    9     177      102     18    57    57.6%      83.6%
total    2007      785    145  1077    39.1%

confusion matrix (rows: actual, columns: detected)
          0    1    2    3    4    5    6    7    8    9 none
    0   308    0    0    0    0    0    0    0    0    6   45
    1     0   99    0    2    0    0    0    0    0    0  163
    2     0    0   25    4    0    0    0    0   14    2  153
    3     2    0    0   90    0    0    0    0    1    3   70
    4     1    0    0   20   12    0    0    0    0    1  166
    5     3    0    0    3    0   26    1    0    0    0  127
    6     4    0    0   10    0    0   50    0    0    0  106
    7     0    7    0    1    0    0    0   26    0    5  108
    8    33    0    0    0    0    0    1    0   47    3   82
    9    13    0    0    4    1    0    0    0    0  102   57

=== training set (1707 images) ===

digit  images  correct  wrong  none   recall  precision
    0     319      274      3    42    85.9%      85.4%
    1     252      109      0   143    43.3%      95.6%
    2     202       26     30   146    12.9%     100.0%
    3     131       80      2    49    61.1%      83.3%
    4     122       11      4   107     9.0%     100.0%
    5      88       23      1    64    26.1%     100.0%
    6     151       45      4   102    29.8%      95.7%
    7     166       27      9   130    16.3%     100.0%
    8     144       45     38    61    31.2%      64.3%
    9     132       77     14    41    58.3%      88.5%
total    1707      717    105   885    42.0%

confusion matrix (rows: actual, columns: detected)
          0    1    2    3    4    5    6    7    8    9 none
    0   274    0    0    0    0    0    1    0    1    1   42
    1     0  109    0    0    0    0    0    0    0    0  143
    2     0    0   26    5    0    0    0    0   23    2  146
    3     0    0    0   80    0    0    0    0    0    2   49
    4     0    0    0    4   11    0    0    0    0    0  107
    5     0    0    0    1    0   23    0    0    0    0   64
    6     0    0    0    4    0    0   45    0    0    0  102
    7     0    5    0    1    0    0    0   27    0    3  130
    8    34    0    0    1    0    0    1    0   45    2   61
    9    13    0    0    0    0    0    0    0    1   77   41

Process finished with exit code 0
```

## Classes
The project makes use of three distinct classes, each with their own purpose. Each will be explored in more detail below,
but in general nodes form the skeleton of the figure, a stroke is a single path through those nodes and a pattern is a
set of steps a stroke is checked against.
### Nodes
Nodes are used to convey how points in the figure are generally connected to each other. After using the Zhang-Suen skeletonizing
algorithm the matrix is converted into a graph, in which every node is a pixel of the skeleton.
### Strokes
A stroke is a single path through the nodes, as found by the tracer. This path is simplified into a polyline using the
Ramer-Douglas-Peucker algorithm, whose vertices are the places where one step of a pattern can end and the next one can begin.
Between any two of these vertices, a `Piece` can be measured: its heading, length, how much it turns and how far it bulges.
These pieces are not a fixed division of the stroke; they are computed on demand while matching.
### Patterns
Patterns are the blueprint of digits. They consist of `Steps`, such as a line, an arc, a semicircle, a corner or a reversal.
Where a piece is a measurement, a step is a condition: it either accepts or rejects a piece. A stroke matches a pattern when
it can be split into consecutive pieces that each pass their step, in either drawing direction. Some patterns additionally
require the stroke to start or end in a certain region of the figure.
## Modules
The project consists of four modules, which are run in order. Specifically:
1. First, the matrix is normalized. We set a cutoff value which allows us to turn a grayscale image into a black-and-white image for easier processing.
2. Then, the matrix is skeletonized using the aforementioned Zhang-Suen algorithm and converted into a graph. This graph is then contracted into key nodes (endpoints and junctions) and the segments between them.
3. Afterwards, the tracer is run on this graph to generate every single way to draw the entire figure in a single stroke, with reversing out of a single dead end still allowed.
4. Finally, the detector is run to see if any of these strokes matches a known pattern. The patterns are tried in fixed order, with the first matching pattern being chosen. If no pattern matches, no digit is detected.
## Patterns
For every digit, at least one pattern is declared. There are, however, digits with multiple ways to write them. A 1, for example,
can be written as a single line and as the one present on most keyboards, with the same holding for digits like fours. For this, multiple patterns can be
defined for a single digit.

