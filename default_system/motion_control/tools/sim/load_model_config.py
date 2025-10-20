import platform
if platform.system() == 'Windows':
    import webbrowser
    webbrowser._tryorder.insert(0,'chrome')

import yaml

# load model configuration
def loadStockerModel():
    modelfile = "../../config/stocker_w_io_model.yml"
    with open(modelfile) as f :
        config = yaml.load(f)["Systems"]["Stocker"]
    return config

def loadOHTModel():
    modelfile = "../../config/oht_eni_model.yml"
    with open(modelfile) as f :
        config = yaml.load(f)["Systems"]["OHT"]
    return config




