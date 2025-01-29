import torch
from groq import Groq
import pickle
from sklearn.ensemble import RandomForestRegressor
import pandas as pd
import numpy as np

def run_model_with_shap(input_dict, model_path="./model", explainer_path="./explainer"):
    with open(model_path, 'rb') as model_file:
        rf_model = pickle.load(model_file)
    with open(explainer_path, 'rb') as explainer_file:
        explainer = pickle.load(explainer_file)

    input_features = np.array([[
        input_dict['sleep_duration']/7.005421995,
        input_dict['physical_activity']/60.647693897,
        input_dict['screen_hours']/5.231657134,
        input_dict['time_indoors']/10.06884195,
        input_dict['noise_level']/49.950499462
    ]])

    prediction = rf_model.predict(input_features)[0]

    shap_values = explainer(input_features)

    shap_values_dict = dict(zip(input_dict.keys(), shap_values.values[0]))

    return prediction, shap_values_dict
    
    
    
def get_recommendation(input_dict):
    api_key = "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"

    client = Groq(api_key=api_key)
    output=run_model_with_shap(input_dict)
    prompt = "Here are the SHAP values for the input features (the effects of them on this particular mental health score) This values indicate what the effects of the input features was on the mental health score. High positive values indicate that the feature had a positive effect. Low negative values mean that the feature had a big negative effect.:\n" + str(output[1]) + "\n\nThe predicted mental health score is: " + str(output[0]) + "\n\nGive recommendations to improve mental health, based on these values, about negative SHAP valued features, to get rid of their negative effect. Do not mention SHAP values. Do not speak technically."
    response = client.chat.completions.create(
        model="llama-3.3-70b-specdec",
        messages=[
            {"role": "user", "content": prompt}
        ],
        max_tokens=128
    )

    generated_text = response.choices[0].message.content
    return generated_text
    
    
    
input_dict2 = {
    "sleep_duration": 2,
    "physical_activity": 6,
    "screen_hours": 1,
    "time_indoors": 5,
    "noise_level": 20
}

print(get_recommendation(input_dict2))



