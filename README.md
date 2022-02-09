<!--
 Copyright (c) 2022 Asaf Zebulon
 
 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# Stochastic Rescaler Algorithm
## Purpose
This algorithm can be used to remove bias or to rescale an input data into predefined maximum value. It learns the amount of bias to remove on the go, by applying similar concept with k-means clustering algorithm. At first, you need to define the cluster initial location. Good initial value to set are {25% of max. expected value, 75% of max.expected value}. For example, if the input data is expected to be in the range of [0, 100], then it is good to set the initial value of the clusters as {25, 75}.

The model (collection of clusters) is updated when given a new input value. It will find the closest cluster to the input value, and it will move the cluster closer to the input value. As more input is given to update the model, new input will have less effect in moving the cluster location. This mimics the mechanism of simulated annealing.


## Performance
The strength of this algorithm is the "unsupervised learning" manner that may help to learn data distribution on-line. The result of this algorithm can be seen on the figure below. Figure below shows how the algorithm performs bias removal from the input signal, while reserving the peak value of the original input data distribution.

<img src="https://github.com/ajebulon/stochastic-rescaler/blob/HEAD/assets/processed.jpg" alt="drawing"/>

Figure below shows how the algorithm remove bias and rescale the peak value into predefined maximum value.

<img src="https://github.com/ajebulon/stochastic-rescaler/blob/HEAD/assets/processed + rescaled.jpg" alt="drawing"/>


## Usage
There is no special package required to compile this. Just install 

`$ sudo apt install -y make`

Clone this repository, and run 

`$ make`

Please refer to `main.c` to learn how to use the algorithm. It covers the procedure to create and initialize the model, update/train the model, and predict the processed input value using trained model.

## License
You can use this anywhere, just don't forget to keep my name. Anyway this repository is MIT licensed, as found in the [LICENSE][l] file.

[l]: https://github.com/ajebulon/stochastic-rescaler/blob/HEAD/LICENSE