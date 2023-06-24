// Oluwasegun Olagoroye
// CS499

//Artifact Number 1

//Inventory Management Mobile Application


import 'package:flutter/material.dart';

void main() {
  runApp(InventoryManagementApp());
}

class InventoryManagementApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Inventory Management',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: InventoryHomePage(),
    );
  }
}

class InventoryHomePage extends StatefulWidget {
  @override
  _InventoryHomePageState createState() => _InventoryHomePageState();
}

class _InventoryHomePageState extends State<InventoryHomePage> {
  List<Product> products = [];

  void addProduct(String name, int quantity) {
    setState(() {
      products.add(Product(name: name, quantity: quantity));
    });
  }

  void removeProduct(int index) {
    setState(() {
      products.removeAt(index);
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Inventory Management'),
      ),
      body: ListView.builder(
        itemCount: products.length,
        itemBuilder: (context, index) {
          return ListTile(
            title: Text(products[index].name),
            subtitle: Text('Quantity: ${products[index].quantity}'),
            trailing: IconButton(
              icon: Icon(Icons.delete),
              onPressed: () {
                removeProduct(index);
              },
            ),
          );
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          showDialog(
            context: context,
            builder: (context) => AddProductDialog(addProduct),
          );
        },
        child: Icon(Icons.add),
      ),
    );
  }
}

class Product {
  final String name;
  final int quantity;

  Product({required this.name, required this.quantity});
}

class AddProductDialog extends StatefulWidget {
  final Function(String, int) addProduct;

  AddProductDialog(this.addProduct);

  @override
  _AddProductDialogState createState() => _AddProductDialogState();
}

class _AddProductDialogState extends State<AddProductDialog> {
  late TextEditingController nameController;
  late TextEditingController quantityController;

  @override
  void initState() {
    super.initState();
    nameController = TextEditingController();
    quantityController = TextEditingController();
  }

  @override
  void dispose() {
    nameController.dispose();
    quantityController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: Text('Add Product'),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          TextField(
            controller: nameController,
            decoration: InputDecoration(
              labelText: 'Product Name',
            ),
          ),
          TextField(
            controller: quantityController,
            decoration: InputDecoration(
              labelText: 'Quantity',
            ),
            keyboardType: TextInputType.number,
          ),
        ],
      ),
      actions: [
        TextButton(
          onPressed: () {
            Navigator.pop(context);
          },
          child: Text('Cancel'),
        ),
        ElevatedButton(
          onPressed: () {
            String name = nameController.text;
            int quantity = int.tryParse(quantityController.text) ?? 0;
            widget.addProduct(name, quantity);
            Navigator.pop(context);
          },
          child: Text('Add'),
        ),
      ],
    );
  }
}
