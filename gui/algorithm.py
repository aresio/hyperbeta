from pylab import *
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from math import acos, pi
from networkx import *
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
		print " * Not saving distance matrix"

	input_folder = args.input_folder
	file_name    = args.file_name

	try:
		GROUPSIZE = int(loadtxt(input_folder+"/grouplength"))
		print " * Using groupsize", GROUPSIZE
	except:
		print "ERROR: cannot retrieve 'grouplength' file in input folder", input_folder
		exit(2)

	# BB_peptide
	# dm500B30M3p1.gro <--- target normale
	# 2fkg_clear       <--- completamente ordinata
	# B26_olig1        <--- completamente disordinata	

	# A = genfromtxt("dm500B30M3p1.gro", skiprows=2, skip_footer=1, dtype={'names': ('ammino', 'boh', 'id', 'x', 'y', 'z'), 'formats': ('S1', 'S2', int, float, float, float) })
	A = genfromtxt(input_folder+"/"+file_name, skip_header=2, skip_footer=1, dtype={'names': ('ammino', 'boh', 'id', 'x', 'y', 'z'), 'formats': ('S1', 'S2', int, float, float, float) })

	B = [ array([x,y,z]) for a,b,c,x,y,z in A ]

	THRESHOLD_DIST=args.distance_threshold		# angstrom
	THRESHOLD_ANG =args.angular_threshold		# 95% pi
	#GROUPSIZE = 11			# 11 = dm500, B26_olig1 = 11, 2fkg = 8
	print " * Using distance", args.distance_threshold, "and angle", args.angular_threshold
	
	G = Graph()

	if precalc:
		D = loadtxt("distances_"+file_name)
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

	lista_supernodi = []	
	for n1, b1 in enumerate(B):				
		for n2, b2 in enumerate(B):
			if n1/GROUPSIZE==n2/GROUPSIZE: continue
			if D[n1][n2]>THRESHOLD_DIST: continue
			if n1==n2: continue
			for n3, b3 in enumerate(B):				
				if n1/GROUPSIZE==n3/GROUPSIZE or n2/GROUPSIZE==n3/GROUPSIZE: continue
				if n2==n3 or n1==n3: continue
				if D[n2][n3]>THRESHOLD_DIST:
					pass
				else:					
					if angle_from_points(b1,b2,b3)>THRESHOLD_ANG:
						lista_supernodi.append(set([n1,n2,n3]))

	for sn1 in lista_supernodi:
		for sn2 in lista_supernodi:
			if len(sn1.intersection(sn2))>1:
				G.add_edge(str(tolist(sn1)), str(tolist(sn2)))

	# print lista_supernodi
	CC = connected_components(G)
	CC = filter(lambda x: len(x)>1, CC)

	"""
	for (x,y,z) in B:
		ax.scatter([x],[y],zs=[z], c="b", alpha=0.5)
	"""
	
	OUTDIR = input_folder+"/output_"+file_name.replace('/', '_').replace(".", "_")
	print " * Saving structures in", OUTDIR
	
	try:
		os.mkdir(OUTDIR)
	except OSError as exception:
		if exception.errno == errno.EEXIST:
			print "WARNING: directry exists"
		else:
			print exception
			print "Unhandled ERROR"; exit(3)
		#shutil.rmtree(OUTDIR)
		#os.mkdir(OUTDIR)		
		pass

	for n,c in enumerate(CC):
		strut = map(lambda x: eval(x), c)
		coords = map(lambda x: [B[x[0]], B[x[1]], B[x[2]]], strut )
		savetxt(OUTDIR+"/struttura"+str(n), strut, fmt="%d\t%d\t%d")
		#savetxt("coordinate"+str(n), coords, fmt="%f\t%f\t%f")
		with open(OUTDIR+"/coordinate"+str(n), "w") as fo:
			for tripla in coords:
				#tripla = matrix(tripla).reshape(3,3)
				T1 = tripla[0]
				T2 = tripla[1]
				T3 = tripla[2]		
				"""
				ax.scatter(T1[0], T1[1], zs=T1[2], c="r")
				ax.scatter(T2[0], T2[1], zs=T2[2], c="r")
				ax.scatter(T3[0], T3[1], zs=T3[2], c="r")
				"""
				fo.write("\t".join(map(str, T1))+"\n")
				fo.write("\t".join(map(str, T2))+"\n")
				fo.write("\t".join(map(str, T3))+"\n")

	print  " *", len(CC), "structures detected"
	savetxt(OUTDIR+"/structures", [len(CC)], fmt="%d")

		#print tripla.T[0][0], tripla.T[1][0], tripla.T[2][0]
		#ax.scatter(tripla.T[0], tripla.T[1], zs=tripla.T[2])
		#ax.scatter(coords)
	#show()

		#ax.scatter()
	#show()
	

