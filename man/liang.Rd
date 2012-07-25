\name{liang}
\alias{liang}

\title{Uniformity Test on the Hypercube}
\description{
Return the p-value of Tn-based test of Liang et al.
}
\usage{
liang(x)
}

\arguments{
  \item{x}{A \code{matrix} of real numbers, the columns of which
  have to be tested for uniformity.}
}

\details{ 
  Liang, Fang, Hickernell and Li (Testing multivariate uniformity
  and its applications, Mathematics of Computation, 70(233), 337–355,
  2000) described a test for uniformity of multidimensional data
  on the hypercube.

  This method computes U-types statistics based on discrepancies and
  combines them into a chi-square statistics.

  The output is the p-value of the chi-square test.
}

\value{
  The p-value of the test.
}

\author{Liang, Fang, Hickernell, Li}
\keyword{multivariate}
\keyword{uniformity}
