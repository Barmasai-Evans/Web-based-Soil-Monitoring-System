import pickle
import numpy as np
from flask import Flask, request, render_template

# Load the trained model
with open("spoilage_model.pkl", "rb") as file:
    model = pickle.load(file)

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/predict', methods=['POST'])
def predict():
    try:
        temperature = float(request.form['temperature'])
        humidity = float(request.form['humidity'])
        
        # Prepare input for the model
        input_features = np.array([[temperature, humidity]])
        prediction = model.predict(input_features)
        
        result = "YES" if prediction[0] == 1 else "NO"
        
        return render_template('index.html', prediction_text=f'Spoilage Prediction: {result}')
    except Exception as e:
        return render_template('index.html', prediction_text=f'Error: {str(e)}')

if __name__ == "__main__":
    app.run(debug=True)