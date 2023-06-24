// Oluwasegun Olagoroye
// CS499

//Artifact Number 1

//Inventory Management Mobile Application




// Importing required packages
import 'package:flutter/material.dart';
import 'package:path/path.dart';
import 'package:sqflite/sqflite.dart';

void main() {
  runApp(InventoryManagementApp());
}

// Main Flutter application class
class InventoryManagementApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    // MaterialApp widget with app title and theme
    return MaterialApp(
      title: 'Inventory Management',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: InventoryHomePage(),
    );
  }
}

// Home page of the inventory management app
class InventoryHomePage extends StatefulWidget {
  @override
  _InventoryHomePageState createState() => _InventoryHomePageState();
}

class _InventoryHomePageState extends State<InventoryHomePage> {
  List<Product> products = [];

  // Method to add a product to the list
  void addProduct(String name, int quantity) {
    setState(() {
      products.add(Product(name: name, quantity: quantity));
      // Save the updated product list to persistent storage
      saveProductList();
    });
  }

  // Method to edit a product in the list
  void editProduct(int index, String name, int quantity) {
    setState(() {
      products[index] = Product(name: name, quantity: quantity);
      // Save the updated product list to persistent storage
      saveProductList();
    });
  }

  // Method to remove a product from the list
  void removeProduct(int index) {
    setState(() {
      products.removeAt(index);
      // Save the updated product list to persistent storage
      saveProductList();
    });
  }

  // Method to save the product list to a SQLite database
  void saveProductList() async {
    final database = await openDatabase(
      join(await getDatabasesPath(), 'inventory_database.db'),
      onCreate: (db, version) {
        return db.execute(
          'CREATE TABLE products(id INTEGER PRIMARY KEY, name TEXT, quantity INTEGER)',
        );
      },
      version: 1,
    );

    // Clear the existing table
    await database.delete('products');

    // Insert the updated product list into the table
    for (var product in products) {
      await database.insert(
        'products',
        product.toMap(),
        conflictAlgorithm: ConflictAlgorithm.replace,
      );
    }
  }

  // Method to load the product list from the SQLite database
  Future<void> loadProductList() async {
    final database = await openDatabase(
      join(await getDatabasesPath(), 'inventory_database.db'),
      onCreate: (db, version) {
        return db.execute(
          'CREATE TABLE products(id INTEGER PRIMARY KEY, name TEXT, quantity INTEGER)',
        );
      },
      version: 1,
    );

    final List<Map<String, dynamic>> maps = await database.query('products');

    setState(() {
      products = List.generate(maps.length, (i) {
        return Product(
          name: maps[i]['name'],
          quantity: maps[i]['quantity'],
        );
      });
    });
  }

  @override
  void initState() {
    super.initState();
    // Load the product list from persistent storage on app start
    loadProductList();
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
            onTap: () {
              // Open edit dialog when the ListTile is tapped
              openEditProductDialog(index);
            },
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

  // Method to open the edit product dialog
  void openEditProductDialog(int index) {
    showDialog(
      context: context,
      builder: (context) => EditProductDialog(
        editProduct: (name, quantity) {
          editProduct(index, name, quantity);
          Navigator.pop(context);
        },
        productName: products[index].name,
        quantity: products[index].quantity,
      ),
    );
  }
}

// Product model class
class Product {
  final String name;
  final int quantity;

  Product({required this.name, required this.quantity});

  Map<String, dynamic> toMap() {
    return {
      'name': name,
      'quantity': quantity,
    };
  }
}

// Dialog for adding a new product
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

// Dialog for editing an existing product
class EditProductDialog extends StatefulWidget {
  final Function(String, int) editProduct;
  final String productName;
  final int quantity;

  EditProductDialog({
    required this.editProduct,
    required this.productName,
    required this.quantity,
  });

  @override
  _EditProductDialogState createState() => _EditProductDialogState();
}

class _EditProductDialogState extends State<EditProductDialog> {
  late TextEditingController nameController;
  late TextEditingController quantityController;

  @override
  void initState() {
    super.initState();
    nameController = TextEditingController(text: widget.productName);
    quantityController = TextEditingController(text: widget.quantity.toString());
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
      title: Text('Edit Product'),
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
            widget.editProduct(name, quantity);
            Navigator.pop(context);
          },
          child: Text('Save'),
        ),
      ],
    );
  }
}
