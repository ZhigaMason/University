## Maksym Khavil's BI-ML2-2025 Repository

**BI-ML1** is a Machine Learning course at [CTU FIT](https://fit.cvut.cz/en), it included two tasks one focuses on dimensionality reduction and other focuses on neural nets for image recognition.

### Homework 01

This task was a binary classification on reduced FashionMNIST dataset, where dresses must have been differentiated from jackets. We have used PCA, QDA and LDA, Naive Bayes, SVM.
Gaussian Discriminant Analysis and Naive Bayes were used to generate artificial images. Test Accuracy is 94.7% and F1 is 0.94.

Methods used:
* Python, Jupyter
* scikit-learn, numpy, pandas, matplotlib
* Naive Bayes, LDA, GDA, SVM, PCA, Locally Linear Embedding

### Homework 02

The task was to check different deeplearning techniques and get an admissible result on FashionMNIST dataset. We have used PyTorch for that, the archtectures that were used are ordinary feedforwards and CNNs. Test accuracy is 90.3%.

Methods used:
* Python, Jupyter
* PyTorch, numpy, pandas, matplotlib
* Feedforward and Convolutional neural nets
* SGD (with momentum), RMSprop, Adam (with weight decay)
* Dropout, L2 regularization, Batch Normalization
* Plateau LR scheduler
