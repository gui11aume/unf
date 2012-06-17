unf = function(x) {
   # Test uniformity of the column vectors of matrix |x|.
   stopifnot(is.matrix(x))
   stopifnot(all(dim(x) > 1))
   return (.Call("uniformity_R_call", x + 0.0))
}
