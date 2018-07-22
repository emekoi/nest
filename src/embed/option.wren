/**
* Copyright (c) 2018 emekoi
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the MIT license. See LICENSE for details.
*/

class Option {
  construct new(value) {
    if (value == null) {
      return Option.none()
    } else {
      Option.some(value)
    }
  }

  construct some(value) {
    _isSome = true
    _value = value
  }
  
  construct none() {
    _isSome = false
  }
  
  toString {
    if (_isSome) {
      return "Some(%(_val))"
    } else {
      return "None"
    }
  }
  
  isSome { _isSome }
  isNone { !isSome }
  
  unwrap() {
    if (this.isNone) {
      Fiber.abort("attempt to unwrap None")
    }
    return _value
  }

  unwrap(msg) {
    if (this.isNone) {
      Fiber.abort(msg)
    }
    return _value
  }

  unwrapOr(default) {
    if (this.isNone) {
      if (default is Fn) {
        if (default.arity != 1) {
          Fiber.abort("block must have arity of 1")
        }
        return default.call()
      } else {
        return default
      }
    return _value
  }

  map(block) {
    if (!(block is Fn)) {
      Fiber.abort("needed Fn got %(block)")
    }
    if (block.arity != 1) {
      Fiber.abort("block must have arity of 1")
    }
    if (this.isSome) { Option.new(block.call(_value)) }
  }

  mapOr(default, block) {
    if (!(block is Fn)) {
      Fiber.abort("needed Fn got %(block)")
    }
    if (block.arity != 1) {
      Fiber.abort("block must have arity of 1")
    }
    if (this.isNone) {
      if (default is Fn) {
        if (default.arity != 1) {
          Fiber.abort("block must have arity of 1")
        }
        return default.call()
      } else {
        return default
      }
    }

    block.call(_value)
  }
}