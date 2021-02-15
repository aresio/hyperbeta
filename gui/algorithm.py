from math import acos, pi
from networkx import connected_components, Graph
from numpy import loadtxt, savetxt, genfromtxt, array
from numpy.linalg import norm
import errno
import os, shutil
import argparse

def angle_from_points(A,B,C):
	v1 = A-B
	v2 = C-B
	v1mag = norm(v1)
	v2mag = norm(v2)
	v1norm = v1/v1mag
	v2norm = v2/v2mag
	res = acos(sum(v1norm*v2norm))
	return res/pi


def tolist(st):	return sorted(list(st))


if __name__ == '__main__':


	parser = argparse.ArgumentParser(description='Arguments.')
	parser.add_argument('input_folder', type=str, help='Input folder')
	parser.add_argument('file_name', type=str, help='.GRO file')	
	parser.add_argument('-a', '--angular_threshold', help='Angular threshold', type=float)
	parser.add_argument('-d', '--distance_threshold', help='Distance threshold', type=float)
	parser.add_argument('-p', '--precalc', help='Use precalculated distances', action="store_true")
	parser.add_argument('-n', '--nosave', help='Do not save distances', action="store_true")
	args = parser.parse_args()
		
	precalc = args.precalc
	nosave  = args.nosave

	if nosave:
		print (" * Not saving distance matrix")

	input_folder = args.input_folder
	file_name    = args.file_name

	try:
		GROUPSIZE = int(loadtxt(input_folder+os.sep+"grouplength"))
		print (" * Using groupsize", GROUPSIZE)
	except FileNotFoundError:
		print( "ERROR: cannot retrieve 'grouplength' file in input folder", input_folder)
		exit(2)

	
	A = genfromtxt(input_folder+os.sep+file_name, skip_header=2, skip_footer=1, dtype={'names': ('ammino', 'boh', 'id', 'x', 'y', 'z'), 'formats': ('S1', 'S2', int, float, float, float) })

	B = [ array([x,y,z]) for a,b,c,x,y,z in A ]

	THRESHOLD_DIST=args.distance_threshold		
	THRESHOLD_ANG =args.angular_threshold		
	print (" * Using distance", args.distance_threshold, "and angle", args.angular_threshold)
	
	G = Graph()

	if precalc:
		D = loadtxt(input_folder+os.sep+"distances_"+file_name)
	else:
		D = []
		for n,b1 in enumerate(B):
			b1 = array(b1)
			temp = []
			for m,b2 in enumerate(B):
				b2 = array(b2)
				res = norm(b1-b2)
				temp.append(res)
			D.append(temp)
		if not nosave: savetxt("distances"+file_name,  D)

	sets_beta_peptides = [set() for _ in range(len(B)//GROUPSIZE+1)]
	
	lista_supernodi = []	
	for n1, b1 in enumerate(B):				
		for n2, b2 in enumerate(B):
			if n1//GROUPSIZE==n2//GROUPSIZE: continue
			if D[n1][n2]>THRESHOLD_DIST: continue
			if n1==n2: continue
			for n3, b3 in enumerate(B):				
				if n1//GROUPSIZE==n3//GROUPSIZE or n2//GROUPSIZE==n3//GROUPSIZE: continue
				if n2==n3 or n1==n3: continue
				if D[n2][n3]>THRESHOLD_DIST:
					pass
				else:					
					if angle_from_points(b1,b2,b3)>THRESHOLD_ANG:
						lista_supernodi.append(set([n1,n2,n3]))
						sets_beta_peptides[n1//GROUPSIZE].add( n1 )

	for sn1 in lista_supernodi:
		for sn2 in lista_supernodi:
			if len(sn1.intersection(sn2))>1:
				G.add_edge(str(tolist(sn1)), str(tolist(sn2)))

	CC = connected_components(G)
	CC = list(filter(lambda x: len(x)>1, CC))

	flattened = []
	for comp in CC:
		for sublist in comp:
			flattened.extend (eval(sublist))
	flattened = sorted(set(flattened))

	pos = 0
	broken = [[]]
	while(pos<len(flattened)-1):
		if flattened[pos]==flattened[pos+1]-1:
			broken[-1].append(flattened[pos])
		else:
			broken[-1].append(flattened[pos])
			broken.append([])
		pos+=1
		if flattened[pos]%GROUPSIZE==0:
			broken.append([])

	groups = list(filter(lambda x: len(x)>1, broken))

	OUTDIR = input_folder+"/output_"+file_name.replace('/', '_').replace(".", "_")
	print (" * Saving structures in", OUTDIR)

	try:
		os.mkdir(OUTDIR)
	except OSError as exception:
		if exception.errno == errno.EEXIST:
			print ("WARNING: directry exists")
		else:
			print (exception)
			print ("Unhandled ERROR"); exit(3)

	for n,c in enumerate(CC):
		strut = [eval(x) for x in c]
		coords = map(lambda x: [B[x[0]], B[x[1]], B[x[2]]], strut )
		savetxt(OUTDIR+"/struttura"+str(n), strut, fmt="%d\t%d\t%d")
		with open(OUTDIR+"/coordinate"+str(n), "w") as fo:
			for tripla in coords:
				T1 = tripla[0]
				T2 = tripla[1]
				T3 = tripla[2]						
				fo.write("\t".join(map(str, T1))+"\n")
				fo.write("\t".join(map(str, T2))+"\n")
				fo.write("\t".join(map(str, T3))+"\n")

	print  (" *", len(CC), "structures detected")
	savetxt(OUTDIR+"/structures", [len(CC)], fmt="%d")
	
	with open(OUTDIR+os.sep+"beta_groups", "w") as fo:
		for group in groups:
			fo.write("\n".join(map(str, group)))
			fo.write("\n\n")
