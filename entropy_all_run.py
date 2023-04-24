import pandas as pd
import numpy as np
import scipy.stats
import sys
'''dataset = '2Q'
tree_criterion = 'random'
leaf_criterion = 'soft_discard'
point_criterion = 'border_points'
sample_size = 5
'''
dataset=sys.argv[1]
tc = ["random"]
lc = ["random","hard_discard","soft_discard"]
pc = ["random","core_points","border_points"]
ss = [1,2,3,4,5]
for sample_size in ss:
	print(str(sample_size)+'%',end=' ')
	for tree_criterion in tc:
		print('tree_criterion='+tree_criterion,end=' ')
		for leaf_criterion in lc:
			print('leaf_criterion='+leaf_criterion,end=' ')
			for point_criterion in pc:
				print('point_criterion='+point_criterion,end=' ')
				print('\n')
				sample_input_string = dataset+'/sampleSets/'+str(sample_size)+'%_'+tree_criterion+'_'+leaf_criterion+'_'+point_criterion+'_sampleSet.csv'
				sample = pd.read_csv(sample_input_string, sep =' ', header = None)
				p_sample_data = sample[1].value_counts()
				entropy = scipy.stats.entropy(p_sample_data, base=len(p_sample_data))
				for count, value in enumerate(p_sample_data):
					print('class',p_sample_data.index[count],'=',value,end=';  ')
				print("\nEntropy:",entropy)

