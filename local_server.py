from flask import Flask, request, jsonify
import json 
app = Flask(__name__)
items = {}
tasks = {}
GLOBAL_STR = "TeichmullerHodge"

@app.route("/items", methods=["GET"])
def get_items():
    return jsonify(items)

@app.route("/items/<int:item_id>", methods=["GET"])
def get_item(item_id):
    item = items.get(item_id)
    if item is None:
        return jsonify({"error": "Item not found"}), 404
    return jsonify(item)

@app.route("/name", methods=["PATCH"])
def change_name():
    print(request.data)
    data = request.json 
    print(json.dumps(data, indent=4))
    GLOBAL_STR = data['Name']
    return jsonify({"Name": GLOBAL_STR}) 

@app.route("/items", methods=["POST"])
def create_item():
    data = request.json
    print(json.dumps(data, indent=4))
    item_id = len(items) + 1
    items[item_id] = data
    return jsonify({"id": item_id, "item": data}), 201

@app.route("/tasks", methods=["PUT"])
def put_item():
    data = request.json
    print(json.dumps(data, indent=4))
    task_id = len(tasks) + 1
    tasks[task_id] = data
    return jsonify({"id": task_id, "tasks": data}), 201


@app.route("/items/<int:item_id>", methods=["PATCH"])
def update_item(item_id):
    if item_id not in items:
        return jsonify({"error": "Item not found"}), 404
    data = request.json
    items[item_id].update(data)
    return jsonify(items[item_id])

@app.route("/items/<int:item_id>", methods=["DELETE"])
def delete_item(item_id):
    if item_id not in items:
        return jsonify({"error": "Item not found"}), 404
    del items[item_id]
    return jsonify({"message": "Item deleted"})


@app.route("/tasks/<int:task_id>", methods=["DELETE"])
def delete_task(task_id):
    if task_id not in tasks:
        return jsonify({"error": "Item not found"}), 404
    del tasks[task_id]
    return jsonify({"message": "Item deleted"})


if __name__ == "__main__":
    app.run(debug=True)
