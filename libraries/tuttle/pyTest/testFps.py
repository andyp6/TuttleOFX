# scons: pluginAudioVideo pluginInvert

from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testFpsPropagation():

	return # TODO

	tuttle.compute( [
		NodeInit( "tuttle.ffmpegreader", filename="TuttleOFX-data/video/morgen-20030714.avi" ),
		NodeInit( "tuttle.invert" ),
		NodeInit( "tuttle.invert" ),
		NodeInit( "tuttle.ffmpegwriter", filename=".tests/output.avi" ),
		] )
	
	# TODO: check the fps! ;)

