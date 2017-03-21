# js-benchmark
ECS 240 Project: Benchmarking Emscripten, Closure and JavaScript by Leonardo Ferrer and Luan Nguyen

The explosion of Web 2.0 has launched a dramatic migration and creation of applications directly on browsers. One of the largest questions in the industry is how to improve the performance of JavaScript in modern browsers. Mozilla and Google have two different answers in the form of Emscripten and Closure respectively. In this work we study how the two compilers behave in terms of performance in cross-browsers tests. The benchmarks include arithmetic, array, string and graphics loads. We present our JavaScript benchmark results from different operating systems and conclude with insights on how to improve the performance of JavaScript. We find that running Google Closure can improve the performance of Javascript on Chrome but would negatively impact the performance on Firefox. Based on our data, we determine the best use of the Closure optimization would be only to reduce code size using the ``SIMPLE'' optimization level. We also find that Javascript code produced through Emscripten compiler has excellent performance for math-heavy applications and poor performance for strings and array intensive operations, irrespective of the browser or operating system. 

## Live versions

Live versions are available in the following locations:

[CCV](http://www.leonardoferrer.com/js-benchmark/CCV/)
[LZMA](http://www.leonardoferrer.com/js-benchmark/LZMA/)
[Box2D-JS](http://www.leonardoferrer.com/js-benchmark/Box2D/)
[Box2D-Closure](http://www.leonardoferrer.com/js-benchmark/Box2D/closure.html)
[Box2D-Emscripten](http://www.leonardoferrer.com/js-benchmark/Box2D/emscripten.html)
[WebGL-JS](http://www.leonardoferrer.com/js-benchmark/Graphics/src)
[WebGL-Closure](http://www.leonardoferrer.com/js-benchmark/Graphics/closure)

Try refreshing the page if a benchmark stalls.


## Source Code

The HTML and JavaScript files are organized per benchmark per compiler. To run the files locally just clone the repository and open the files in the browser of your choice. For convenience, we have provided launch scripts for Chrome in Windows and Ubuntu. Closure is bundled in the repository and compiled versions of Emscripten libraries are included. An installation of Emscripten is not needed to run the benchmarks. 

