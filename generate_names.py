
import random

node_ids = set()
with open("data/facebook_combined.txt", "r") as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith("
            continue
        parts = line.split()
        if len(parts) >= 2:
            node_ids.add(int(parts[0]))
            node_ids.add(int(parts[1]))

node_ids = sorted(node_ids)
print(f"Total unique nodes: {len(node_ids)}")

first_names = [
    "Aarav", "Aanya", "Aditya", "Aditi", "Akash", "Amara", "Amit", "Amrita",
    "Ananya", "Anika", "Ankit", "Ankita", "Arjun", "Arya", "Ayaan", "Bhavya",
    "Chetan", "Daksh", "Deepak", "Devi", "Dhruv", "Divya", "Esha", "Gaurav",
    "Gauri", "Harsh", "Himani", "Isha", "Ishaan", "Jaya", "Kabir", "Karan",
    "Kavya", "Kiara", "Krish", "Kriti", "Krishna", "Laksh", "Lavanya", "Manav",
    "Maya", "Meera", "Mohit", "Naina", "Nandini", "Navya", "Neha", "Nikhil",
    "Nisha", "Nitya", "Omkar", "Pallavi", "Pooja", "Prachi", "Pranav", "Priya",
    "Raghav", "Rahul", "Rashi", "Ravi", "Reyansh", "Rhea", "Rishi", "Ritika",
    "Rohan", "Riya", "Sagar", "Sahil", "Sakshi", "Samar", "Sanya", "Sara",
    "Saurabh", "Shreya", "Shubham", "Simran", "Sneha", "Sonal", "Tanvi", "Tara",
    "Varun", "Vedant", "Vihaan", "Vivaan", "Vivek", "Yash", "Zara", "Zoya",
    "Aarushi", "Abhinav", "Aishwarya", "Ajay", "Alok", "Anand", "Aniket", "Anjali",
    "Aparna", "Arnav", "Ashwin", "Avni", "Ayush", "Charvi", "Dev", "Diya",
    "Garv", "Gautam", "Hansika", "Hardik", "Harini", "Hemant", "Ishan", "Janvi",
    "Jatin", "Keya", "Kunal", "Lakshmi", "Manan", "Mansi", "Mihir", "Mira",
    "Moksh", "Naman", "Nitin", "Ojas", "Paras", "Pihu", "Piyush", "Rachit",
    "Rajat", "Rakesh", "Ria", "Rudra", "Ruhi", "Saanvi", "Sameer", "Sanvi",
    "Shivani", "Siddharth", "Soham", "Srishti", "Swara", "Tanya", "Tejas", "Trisha",
    "Uday", "Utkarsh", "Vanya", "Viraj", "Yashvi", "Yuvan", "Aarna", "Advait",
    "Ahaan", "Anvi", "Atharv", "Avyan", "Darsh", "Eshaan", "Hriday", "Inaya",
]

last_names = [
    "Sharma", "Gupta", "Singh", "Verma", "Patel", "Kumar", "Joshi", "Mehta",
    "Shah", "Mishra", "Agarwal", "Chauhan", "Kapoor", "Malhotra", "Bhat", "Reddy",
    "Nair", "Iyer", "Menon", "Saxena", "Pandey", "Srivastava", "Khanna", "Bansal",
    "Arora", "Chopra", "Datta", "Jain", "Tiwari", "Yadav", "Bhatt", "Rao",
    "Desai", "Pillai", "Das", "Roy", "Bose", "Sen", "Mukherjee", "Chowdhury",
    "Kulkarni", "Deshpande", "Patil", "Kaur", "Gill", "Dhawan", "Bajaj", "Sethi",
    "Ahuja", "Tandon", "Bhatia", "Oberoi", "Sood", "Rajan", "Vyas", "Naik",
    "Hegde", "Nayak", "Shukla", "Dubey", "Trivedi", "Thakur", "Choudhary", "Goyal",
    "Kashyap", "Mittal", "Saini", "Tyagi", "Rawat", "Garg",
]

all_names = []
for first in first_names:
    for last in last_names:
        all_names.append(f"{first} {last}")

random.seed(42)
random.shuffle(all_names)

assert len(all_names) >= len(node_ids), f"Need {len(node_ids)} names but only have {len(all_names)}"

with open("data/names.txt", "w") as f:
    for i, nid in enumerate(node_ids):
        f.write(f"{nid} {all_names[i]}\n")

print(f"Generated {len(node_ids)} names -> data/names.txt")
print("Sample entries:")
for nid in node_ids[:5]:
    print(f"  {nid} -> {all_names[node_ids.index(nid)]}")

