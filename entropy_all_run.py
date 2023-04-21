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
tree_criterion = sys.argv[2]
leaf_criterion = sys.argv[3]
point_criterion = sys.argv[4]
sample_size = sys.argv[5]
sample_input_string = dataset+'/sampleSets/'+str(sample_size)+'%_'+tree_criterion+'_'+leaf_criterion+'_'+point_criterion+'_sampleSet.csv'

sample = pd.read_csv(sample_input_string, sep =' ', header = None)

p_sample_data = sample[1].value_counts()
entropy = scipy.stats.entropy(p_sample_data, base=len(p_sample_data))

print(dataset+'/sampleSets/'+str(sample_size)+'%_'+tree_criterion+'_'+leaf_criterion+'_'+point_criterion)
for count, value in enumerate(p_sample_data):
	print('class',p_sample_data.index[count],'=',value,end=';  ')
print("\nEntropy:",entropy)

