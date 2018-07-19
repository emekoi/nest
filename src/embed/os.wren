/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


class Platform {
  foreign static name
  foreign static isPosix
  foreign static exeDir
	foreign static appData

  static isWindows { name == "windows" }
}

class Process {
  foreign static allArguments

  static arguments { allArguments[2..-1] }
}
