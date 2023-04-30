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
ss = [5]
numRuns = 5

data_input_string = dataset+'/'+dataset+'.csv'
data = pd.read_csv(data_input_string, sep =' ', header = None)
data =data.set_index(0)
p_data = data[len(data.columns)].value_counts()
entropy = scipy.stats.entropy(p_data, base=len(p_data))
print('Method','Run',end=' ')
for count, value in enumerate(p_data):
	print('class='+str(p_data.index[count]),end=' ')	
print('')
print('- Complete-Data',end=' ')
for count, value in enumerate(p_data):
	print(value,end=' ')	
print(entropy)


for sample_size in ss:
	#print(str(sample_size)+'%',end='-')
	for tree_criterion in tc:
		#print('tree_criterion='+tree_criterion,end=' ')
		for leaf_criterion in lc:
			#print('leaf_criterion='+leaf_criterion,end=' ')
			for point_criterion in pc:
				#print('point_criterion='+point_criterion,end=' ')
				#print('\n')
				print('\n'+str(sample_size)+'%-'+tree_criterion+'-'+leaf_criterion+'-'+point_criterion,end=' ')		
					
				for run in range(1,6):
					print(' '+str(run),end=' ')
					sample_input_string = dataset+'/sampleSets/'+str(run)+'_Run'+str(sample_size)+'%_'+tree_criterion+'_'+leaf_criterion+'_'+point_criterion+'_sampleSet.csv'
					sample = pd.read_csv(sample_input_string, sep =' ', header = None)
					p_sample_data = sample[1].value_counts()
					entropy = scipy.stats.entropy(p_sample_data, base=len(p_sample_data))
					for dcount, dvalue in enumerate(p_data):
						for count, value in enumerate(p_sample_data):
							#print("dfdfd",p_data.index[dcount],p_sample_data.index[count])	
							if p_data.index[dcount] == p_sample_data.index[count]:
								print(value,end=' ')
					#print("\nEntropy:",entropy)
					print(entropy)






















