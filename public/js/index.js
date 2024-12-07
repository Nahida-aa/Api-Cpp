document.addEventListener('DOMContentLoaded', function() {
    const token = localStorage.getItem('token');
    if (token) {
        showWelcome();
    } else {
        showLogin();
    }

    document.getElementById('loginForm').addEventListener('submit', function(event) {
        event.preventDefault();
        const formData = new FormData(event.target);
        const data = {
            id_number: formData.get('idNumber'),
            name: formData.get('name'),
            gender: formData.get('gender'),
            birth_date: formData.get('birthDate')
        };
        fetch('/api/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        })
        .then(response => response.json())
        .then(result => {
            if (result.token) {
                localStorage.setItem('token', result.token); // 为了简化, 这里直接将 token 存储在 localStorage 中
                showWelcome();
            } else {
                showMessage('登录失败', 'error');
            }
        })
        .catch(error => {
            showMessage('登录失败: ' + error.message, 'error');
        });
    });

    document.getElementById('logoutButton').addEventListener('click', function() {
        localStorage.removeItem('token');
        showLogin();
    });

    document.getElementById('resetFilter').addEventListener('click', function() {
        document.getElementById('filterFlightNumber').value = '';
        document.getElementById('filterDeparture').value = '';
        document.getElementById('filterArrival').value = '';
        document.getElementById('filterDate').value = '';
        fetchFlights();
    });

    document.getElementById('filterFlightNumber').addEventListener('input', applyFilters);
    document.getElementById('filterDeparture').addEventListener('input', applyFilters);
    document.getElementById('filterArrival').addEventListener('input', applyFilters);
    document.getElementById('filterDate').addEventListener('input', applyFilters);

    document.getElementById('notificationButton').addEventListener('click', function() {
        const notificationContainer = document.getElementById('notificationContainer');
        if (notificationContainer.style.display === 'none') {
            notificationContainer.style.display = 'block';
            const token = localStorage.getItem('token');
            fetch('/api/notifications/read', {
                method: 'PUT',
                headers: {
                    'Authorization': token
                }
            })
            .then(() => {
                // document.getElementById('notificationDot').style.display = 'none';
                const notificationCount = document.querySelector('.notification-count');
                if (notificationCount) {
                    notificationCount.remove();
                }
            })
            .catch(error => {
                showMessage('标记通知已读失败: ' + error.message, 'error');
            });
        } else {
            notificationContainer.style.display = 'none';
        }
    });
});

function showLogin() {
    document.getElementById('loginContainer').style.display = 'block';
    document.getElementById('welcomeContainer').style.display = 'none';
}

function showWelcome() {
    const token = localStorage.getItem('token'); 
    const payload = JSON.parse(atob(token.split('.')[1]));
    document.getElementById('welcomeMessage').textContent = `欢迎, ${payload.id_number}`;
    document.getElementById('loginContainer').style.display = 'none';
    document.getElementById('welcomeContainer').style.display = 'block';
    fetchFlights();
    fetchBookings();
    fetchNotifications();
}

function fetchFlights() {
    fetch('/api/flights')
    .then(response => response.json().then(data => ({ status: response.status, body: data })))
    .then(result => {
        if (result.status === 200) {
            updateFlightsTable(result.body);
        } else {
            showMessage('获取航班列表失败: ' + result.body.error, 'error');
        }
    })
    .catch(error => {
        showMessage('获取航班列表失败: ' + error.message, 'error');
    });
}

function fetchBookings() {
    const token = localStorage.getItem('token');
    fetch('/api/bookings', {
        method: 'GET',
        headers: {
            'Authorization': token
        }
    })
    .then(response => response.json().then(data => ({ status: response.status, body: data })))
    .then(result => {
        if (result.status === 200) {
            updateBookingsTable(result.body);
        } else {
            showMessage('获取订票列表失败: ' + result.body.error, 'error');
        }
    })
    .catch(error => {
        showMessage('获取订票列表失败: ' + error.message, 'error');
    });
}

function fetchNotifications() {
    const token = localStorage.getItem('token');
    fetch('/api/notifications', {
        method: 'GET',
        headers: {
            'Authorization': token
        }
    })
    .then(response => response.json().then(data => ({ status: response.status, body: data })))
    .then(result => {
        if (result.status === 200) {
            updateNotifications(result.body);
        } else {
            showMessage('获取通知失败: ' + result.body.error, 'error');
        }
    })
    .catch(error => {
        showMessage('获取通知失败: ' + error.message, 'error');
    });
}

function updateFlightsTable(flights) {
    const flightsTableBody = document.getElementById('flightsTable').querySelector('tbody');
    flightsTableBody.innerHTML = '';
    flights.forEach(flight => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${flight.flight_number}</td>
            <td>${flight.max_passengers}</td>
            <td>${flight.departure_location}</td>
            <td>${flight.departure_time}</td>
            <td>${flight.arrival_location}</td>
            <td>${flight.arrival_time}</td>
            <td>${flight.price}</td>
            <td>${flight.available_seats}</td>
            <td>${flight.is_cancelled ? '已取消' : '正常'}</td>
            <td>
                ${flight.is_cancelled ? '' : `<button onclick="bookFlight('${flight.flight_number}')">订票</button>`}
            </td>
        `;
        flightsTableBody.appendChild(row);
    });
}

function updateBookingsTable(bookings) {
    const bookingsTableBody = document.getElementById('bookingsTable').querySelector('tbody');
    bookingsTableBody.innerHTML = '';
    bookings.forEach(booking => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${booking.flight_number}</td>
            <td>${booking.seat_number}</td>
            <td>${booking.departure_location}</td>
            <td>${booking.departure_time}</td>
            <td>${booking.arrival_location}</td>
            <td>${booking.arrival_time}</td>
            <td>
                <button onclick="cancelBooking('${booking.flight_number}', '${booking.seat_number}')">退票</button>
            </td>
        `;
        bookingsTableBody.appendChild(row);
    });
}

function updateNotifications(notifications) {
    const notificationList = document.getElementById('notificationList');
    notificationList.innerHTML = '';
    notifications.forEach(notification => {
        const listItem = document.createElement('li');
        listItem.textContent = notification.msg;
        notificationList.appendChild(listItem);
    });
    const unreadCount = notifications.filter(n => !n.is_read).length;
    const notificationButton = document.getElementById('notificationButton');
    if (unreadCount > 0) {
        const notificationCount = document.createElement('span');
        notificationCount.className = 'notification-count';
        notificationCount.textContent = unreadCount;
        notificationButton.appendChild(notificationCount);
    }
}

function applyFilters() {
    const flightNumber = document.getElementById('filterFlightNumber').value.toLowerCase();
    const departure = document.getElementById('filterDeparture').value.toLowerCase();
    const arrival = document.getElementById('filterArrival').value.toLowerCase();
    const date = document.getElementById('filterDate').value;

    fetch('/api/flights')
    .then(response => response.json().then(data => ({ status: response.status, body: data })))
    .then(result => {
        if (result.status === 200) {
            let filteredFlights = result.body;
            if (flightNumber) {
                filteredFlights = filteredFlights.filter(f => f.flight_number.toLowerCase().includes(flightNumber));
            }
            if (departure) {
                filteredFlights = filteredFlights.filter(f => f.departure_location.toLowerCase().includes(departure));
            }
            if (arrival) {
                filteredFlights = filteredFlights.filter(f => f.arrival_location.toLowerCase().includes(arrival));
            }
            if (date) {
                filteredFlights = filteredFlights.filter(f => f.departure_time.startsWith(date));
            }
            updateFlightsTable(filteredFlights);
        } else {
            showMessage('获取航班列表失败: ' + result.body.error, 'error');
        }
    })
    .catch(error => {
        showMessage('获取航班列表失败: ' + error.message, 'error');
    });
}

function bookFlight(flightNumber) {
    const seatNumber = prompt("请输入座位号:");
    if (!seatNumber) {
        showMessage('订票失败: 座位号不能为空', 'error');
        return;
    }
    // 变整数
    seatNumber = parseInt(seatNumber);
    const token = localStorage.getItem('token');
    fetch('/api/bookings', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': token
        },
        body: JSON.stringify({
            flight_number: flightNumber,
            seat_number: seatNumber,
            num_tickets: 1
        })
    })
    .then(response => response.json())
    .then(result => {
        if (result.message === "订票成功") {
            showMessage('订票成功', 'success');
            fetchFlights();
            fetchBookings();
        } else {
            showMessage('订票失败: ' + result.error, 'error');
        }
    })
    .catch(error => {
        showMessage('订票失败: ' + error.message, 'error');
    });
}

function cancelBooking(flightNumber, seatNumber) {
    const token = localStorage.getItem('token');
    fetch('/api/bookings', {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': token
        },
        body: JSON.stringify({
            flight_number: flightNumber,
            seat_number: seatNumber
        })
    })
    .then(response => response.json())
    .then(result => {
        if (result.message === "退票成功") {
            showMessage('退票成功', 'success');
            fetchBookings();
        } else {
            showMessage('退票失败: ' + result.error, 'error');
        }
    })
    .catch(error => {
        showMessage('退票失败: ' + error.message, 'error');
    });
}

function showMessage(message, type) {
    const messageDiv = document.getElementById('message');
    messageDiv.textContent = message;
    messageDiv.className = 'message ' + type;
    messageDiv.style.display = 'block';
    setTimeout(() => {
        messageDiv.style.display = 'none';
    }, 3000);
}