_let countdown = _fun(countdown)
 _fun(n)
 _if n == 0
 _then 0
 _else countdown ( countdown ) (n + -1)
_in countdown ( countdown ) (1000000)
