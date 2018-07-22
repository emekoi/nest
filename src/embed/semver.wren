/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

import "option" for Option

class VersionParser {
	static toNumber(number) {
		Option.new(Num.fromString(number))
	}

	static parseMajor(data) {
		if (data.count > 2 && data[0..1] == "00") {
			return Option.none()
		}
		return data.toNumber()
	}

	static parseMinor(data) {
		if (data.count > 2 && data[0..1] == "00") {
			return Option.none()
		}
		return data.toNumber()
	}

	static parsePatch(data) {
		if (data.count > 2 && data[0..1] == "00") {
			return Option.none()
		}
		return data.toNumber()
	}

	static parsePreTag(data) {
		if (false) {}
	}

	static parseMeta(data) {
		if (false) {}
	}
}

class Version {
	construct new(version) {
		var start = 0
		for (idx in version.count) {
			if (char in 0x30..0x39) {

			}
		}
	}

	major { _major }
	minor { _minor }
	patch { _patch }
	preTag { _preTag }
	meta { _meta }

	toString { "%().%().%()" }
}