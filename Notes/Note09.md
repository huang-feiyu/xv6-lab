# Concurrency Revisited

* a common pattern used in `bio`:
  one lock for the set of items, plus one lock per item.
* a data object may be protected from concurrency in different ways at different
  points in its lifetime, and the protection may take the form of implicit
  structure rather than explicit locks

