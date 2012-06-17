\name{unf}
\alias{unf}

\title{Multidimensional Uniformity Test}
\description{
Return the p-value of MST-based test of Smith and Jain.
}
\usage{
unf(x)
}

\arguments{
  \item{x}{A \code{matrix} of real numbers, the columns of which
  have to be tested for uniformity.}
}

\details{ 
  Smith and Jain (Testing for uniformity in multidimensional data.
  IEEE Transactions Pattern Anal. Machine Intelligence, 6(1), 73–80,
  1984) described a test for uniformity of multidimensional data
  based on MST (minimal spanning tree).

  This method computes the MST of the column vectors of \code{x}
  and uses it to test their uniformity within an approximate hull.

  The result is output as the probability of a t-test.
}

\value{
  The p-value of the test.
}

\author{Ben Pfaff}
\keyword{multivariate}
\keyword{uniformity}
