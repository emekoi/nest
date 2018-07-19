/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

class FS {
	foreign static mount(path)
  foreign static unmount(path)
  foreign static setWritePath(path)
  foreign static exists(file)
  foreign static getSize(file)
  foreign static getModified(file)
  foreign static read(file)
  foreign static isDir(file)
  foreign static listDir(path)
  foreign static write(file, data)
  foreign static append(file, data)
  foreign static delete(file)
  foreign static makeDirs(path)
}