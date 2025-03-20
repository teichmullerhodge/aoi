from flask import Flask, request, jsonify

app = Flask(__name__)
items = {}

@app.route("/items", methods=["GET"])
def get_items():
    return jsonify(items)

@app.route("/items/<int:item_id>", methods=["GET"])
def get_item(item_id):
    item = items.get(item_id)
    if item is None:
        return jsonify({"error": "Item not found"}), 404
    return jsonify(item)

@app.route("/items", methods=["POST"])
def create_item():
    data = request.json
    item_id = len(items) + 1
    items[item_id] = data
    return jsonify({"id": item_id, "item": data}), 201

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

if __name__ == "__main__":
    app.run(debug=True)
