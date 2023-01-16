import lcm

from potatolcm import emocion_usuario

user_input = []

def lcm_handler(channel, data):
    global emocionUsu, user_input, lcm_emocion
    if channel == "EMOCIONUSUARIO":
        global emocionUsu
        emocionUsu = emocion_usuario().decode(data)
        user_input = emocionUsu.emocion
        lcm_emocion = True

lc = lcm.LCM("udpm://239.255.67.67:6767?ttl=1")
sub_emocion = lc.subscribe("EMOCIONUSUARIO",lcm_handler)


while user_input != quit:
	user_input = quit
	try:
		lcm_emocion = False
		while not lcm_emocion:
		    lc.handle()
	except EOFError:
		print(user_input)
		lc.unsubscribe(sub_emocion)
	except KeyboardInterrupt:
		pass
	if user_input:
		while user_input[-1] in "!.":
		    user_input = user_input[:-1]

		
		print("")
		print("Emocion: ", user_input)
