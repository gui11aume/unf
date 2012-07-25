liang <- function(z) {
   # The column vectors of 'x'.
   stopifnot (is.matrix(z))
   stopifnot (all(z >= 0) && all(z <= 1))
   d <- nrow(z)
   n <- ncol(z)

   # Convenience macro.
   colProds <- function(x) exp(colSums(log(x)))

   cindex <- combn(n, 2)
   mat1 <- z[,cindex[1,]]
   mat2 <- z[,cindex[2,]]

   U1 <- sum(colProds(1 + 2*z - 2*z^2))/n
   U2 <- 2^(d+1) * sum(colProds(1 - abs(mat1-mat2))) / (n*(n-1))
   U <- c(U1, U2) - (4/3)^d

   zeta1 <- (9/5)^d - (16/9)^d
   zeta2 <- 2^d - (16/9)^d
   sigma_n <- matrix(
       c(zeta1, 2*zeta1, 2*zeta1, 4*(n-2)*zeta1/(n-1) + 2*zeta2/(n-1)),
       ncol = 2
   )

   Tn <- n * as.vector(t(U) %*% solve(sigma_n) %*% U)
   return (pchisq(q=Tn, df=2, lower.tail=FALSE))
}
