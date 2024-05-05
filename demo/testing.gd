extends GDPine


# Called when the node enters the scene tree for the first time.
func _ready():
	print("readying")
	self._init_default_ipc(DefaultConfigOption.PCSX2)
	print("ipc initialized")
	print(self.ReadMany(0x00B16DD0, 1))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
